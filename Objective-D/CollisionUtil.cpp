#include "CollisionUtil.h"
#include "CameraUtil.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"
#include "TransformUtil.h"
#include "GameResource.h"
#include "Config.h"
#include <thread>
#include <future>

void OOBB::SetUpdateFrequency(int FPS) {
	UpdateFPS = FPS;
	DestDelayTime = 1.0 / (float)FPS;
}

void OOBB::UpdateDelta(float Delta) {
	DeltaTime += Delta;
	if (DeltaTime >= DestDelayTime) {
		DeltaTime -= DestDelayTime;
		DestPassCount++;
	}
}

// 충돌 처리를 담당하는 유틸이다.
// 서로 다른 종류의 바운딩 객체와도 비교 가능하며, 객체가 가지는 위치, 회전, 크기를 파라미터에 넣어주면 된다.
void OOBB::UpdateAnimated(FBXMesh& Mesh, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix, int NodeIndex) {
	if(NodeIndex > Mesh.MeshPart.size() - 1)
		Update(Mesh.MeshPart[0], TMatrix, RMatrix, SMatrix, true);
	else
		Update(Mesh.MeshPart[NodeIndex], TMatrix, RMatrix, SMatrix, true);
}

void OOBB::UpdateAnimated(FBX& TargetFBX, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix, int NodeIndex) {
	if (TargetFBX.GetMeshCount() - 1 < NodeIndex) return;

	if (UpdateFPS != 0) {
		if (PrevDestPassCount == DestPassCount)
			return;

		PrevDestPassCount = DestPassCount;
	}

	Mesh* MeshNode = TargetFBX[NodeIndex];
	const XMFLOAT3* Positions = reinterpret_cast<const XMFLOAT3*>(TargetFBX.PositionMapped[NodeIndex]);
	int VertexCount = MeshNode->Vertices;

	// 병렬 처리
	const int threadCount = std::thread::hardware_concurrency();
	const int verticesPerThread = (VertexCount + threadCount - 1) / threadCount;

	std::vector<std::future<std::vector<XMFLOAT3>>> futures;

	for (int t = 0; t < threadCount; ++t) {
		futures.emplace_back(std::async(std::launch::async, [=]() -> std::vector<XMFLOAT3> {
			int start = t * verticesPerThread;
			int end = std::min(start + verticesPerThread, VertexCount);

			std::vector<XMFLOAT3> partial(end - start);
			std::memcpy(partial.data(), &Positions[start], sizeof(XMFLOAT3) * (end - start));
			return partial;
		}));
	}

	// 모든 정점 취합
	std::vector<XMFLOAT3> AllPositions;
	AllPositions.reserve(VertexCount);
	for (auto& f : futures) {
		std::vector<XMFLOAT3> partial = f.get();
		AllPositions.insert(AllPositions.end(), partial.begin(), partial.end());
	}

	// OOBB 생성
	DirectX::BoundingOrientedBox NewBox;
	BoundingOrientedBox::CreateFromPoints(NewBox, static_cast<UINT>(AllPositions.size()), AllPositions.data(), sizeof(XMFLOAT3));

	// S * R * T 적용
	XMMATRIX S = XMLoadFloat4x4(&SMatrix);
	XMMATRIX R = XMLoadFloat4x4(&RMatrix);
	XMMATRIX T = XMLoadFloat4x4(&TMatrix);
	XMMATRIX M = S * R * T;
	NewBox.Transform(oobb, M);

	XMStoreFloat4(&oobb.Orientation, XMQuaternionNormalize(XMLoadFloat4(&oobb.Orientation)));
}

void OOBB::Update(Mesh* MeshPtr, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix, bool ApplySkinning) {
	if (ApplySkinning) {
		DirectX::BoundingOrientedBox NewBox;
		BoundingOrientedBox::CreateFromPoints(NewBox, MeshPtr->Vertices, (const XMFLOAT3*)MeshPtr->Position, sizeof(XMFLOAT3));

		XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&SMatrix), XMLoadFloat4x4(&RMatrix));
		ResultMatrix = XMMatrixMultiply(ResultMatrix, XMLoadFloat4x4(&TMatrix));
		NewBox.Transform(oobb, ResultMatrix);
		XMStoreFloat4(&oobb.Orientation, XMQuaternionNormalize(XMLoadFloat4(&oobb.Orientation)));
	}

	else {
		XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&SMatrix), XMLoadFloat4x4(&RMatrix));
		ResultMatrix = XMMatrixMultiply(ResultMatrix, XMLoadFloat4x4(&TMatrix));
		MeshPtr->OOBB.Transform(oobb, ResultMatrix);
		XMStoreFloat4(&oobb.Orientation, XMQuaternionNormalize(XMLoadFloat4(&oobb.Orientation)));
	}
}

void OOBB::Update(XMFLOAT3& Position, XMFLOAT3& Size, XMFLOAT3& Rotation) {
	oobb.Center = Position;
	oobb.Extents = Size;
	XMVECTOR Quarternion =
		XMQuaternionRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	XMStoreFloat4(&oobb.Orientation, Quarternion);
}

void OOBB::Render() {
//#ifdef SHOW_BOUND_BOX
	if (!GLOBAL.RENDER_BOUND)
		return;

	GlobalCommandList->SetGraphicsRootSignature(BoundboxShaderRootSignature);

	Transform::Identity(TranslateMatrix);
	Transform::Identity(ScaleMatrix);

	Transform::Move(TranslateMatrix, oobb.Center.x, oobb.Center.y, oobb.Center.z);
	Transform::Scale(ScaleMatrix, oobb.Extents.x, oobb.Extents.y, oobb.Extents.z);

	// 쿼터니언을 회전행렬로 변환
	XMVECTOR QuaternionForMatrix = XMLoadFloat4(&oobb.Orientation);
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(QuaternionForMatrix);
	XMStoreFloat4x4(&RotateMatrix, rotationMatrix);

	BoundboxShader->RenderWireframe(GlobalCommandList);
	camera.SetToDefaultMode();

	XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), XMLoadFloat4x4(&RotateMatrix));
	ResultMatrix = XMMatrixMultiply(ResultMatrix, XMLoadFloat4x4(&TranslateMatrix));

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(GlobalCommandList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &BoundboxColor, GAME_OBJECT_INDEX, 3, 16);

	SYSRES.BoundMesh->Render(GlobalCommandList);
//#endif
}


bool OOBB::CheckCollision(const OOBB& Other) {
	if (oobb.Intersects(Other.oobb)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool OOBB::CheckCollision(const AABB& Other) {
	if (oobb.Intersects(Other.aabb)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool OOBB::CheckCollision(const BoundSphere& Other) {
	if (oobb.Intersects(Other.sphere)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}



void AABB::Update(XMFLOAT3& Position, XMFLOAT3& Size) {
	aabb.Center = Position;
	aabb.Extents = Size;
}

void AABB::Render() {
	if (!GLOBAL.RENDER_BOUND)
		return;
	GlobalCommandList->SetGraphicsRootSignature(BoundboxShaderRootSignature);

	Transform::Identity(TranslateMatrix);
	Transform::Identity(ScaleMatrix);

	Transform::Move(TranslateMatrix, aabb.Center.x, aabb.Center.y, aabb.Center.z);
	Transform::Scale(ScaleMatrix, aabb.Extents.x, aabb.Extents.y, aabb.Extents.z);

	BoundboxShader->RenderWireframe(GlobalCommandList);
	camera.SetToDefaultMode();

	XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), XMLoadFloat4x4(&TranslateMatrix));

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(GlobalCommandList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &BoundboxColor, GAME_OBJECT_INDEX, 3, 16);

	SYSRES.BoundMesh->Render(GlobalCommandList);
}

bool AABB::CheckCollision(const AABB& Other) {
	if (aabb.Intersects(Other.aabb)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool AABB::CheckCollision(const OOBB& Other) {
	if (aabb.Intersects(Other.oobb)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool AABB::CheckCollision(const BoundSphere& Other) {
	if (aabb.Intersects(Other.sphere)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool AABB::CheckCollisionPoint(xmfloat2& Point) {
	xmfloat3 input = xmfloat3(Point.x, Point.y, 0.0);
	XMVECTOR pointVec = XMLoadFloat3(&input);
	if (aabb.Contains(pointVec) == DirectX::CONTAINS)
		return true;
	return false;
}



void BoundSphere::Update(const XMFLOAT3& Center, float SizeValue) {
	sphere.Center = Center;
	sphere.Radius = SizeValue * 0.5;
	Size = SizeValue;
}

bool BoundSphere::CheckCollision(const BoundSphere& Other) {
	if (sphere.Intersects(Other.sphere)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool BoundSphere::CheckCollision(const AABB& Other) {
	if (sphere.Intersects(Other.aabb)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

bool BoundSphere::CheckCollision(const OOBB& Other) {
	if (sphere.Intersects(Other.oobb)) {
		Collide = true;
		return true;
	}

	Collide = false;
	return false;
}

void BoundSphere::Render() {
	if (!GLOBAL.RENDER_BOUND)
		return;
	GlobalCommandList->SetGraphicsRootSignature(BoundboxShaderRootSignature);

	Transform::Identity(TranslateMatrix);
	Transform::Identity(ScaleMatrix);

	Transform::Move(TranslateMatrix, sphere.Center.x, sphere.Center.y, sphere.Center.z);
	Transform::Scale(ScaleMatrix, Size * 0.135, Size * 0.135, Size * 0.135);

	BoundboxShader->RenderDefault(GlobalCommandList);
	camera.SetToDefaultMode();

	XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), XMLoadFloat4x4(&TranslateMatrix));

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(GlobalCommandList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &BoundboxColor, GAME_OBJECT_INDEX, 3, 16);

	SYSRES.BoundingSphereMesh->Render(GlobalCommandList);

}