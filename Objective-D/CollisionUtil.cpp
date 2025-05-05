#include "CollisionUtil.h"
#include "CameraUtil.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"
#include "TransformUtil.h"
#include "GameResource.h"
#include "Config.h"

// 충돌 처리를 담당하는 유틸이다.
// 서로 다른 종류의 바운딩 객체와도 비교 가능하며, 객체가 가지는 위치, 회전, 크기를 파라미터에 넣어주면 된다.
void OOBB::UpdateAnimated(FBXMesh& Mesh, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix, int NodeIndex) {
	if(NodeIndex > Mesh.MeshPart.size() - 1)
		Update(Mesh.MeshPart[0], TMatrix, RMatrix, SMatrix, true);
	else
		Update(Mesh.MeshPart[NodeIndex], TMatrix, RMatrix, SMatrix, true);
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
#ifdef SHOW_BOUND_BOX
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

	SysRes.BoundMesh->Render(GlobalCommandList);
#endif
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
#ifdef SHOW_BOUND_BOX
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

	SysRes.BoundMesh->Render(GlobalCommandList);
#endif
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
#ifdef SHOW_BOUND_BOX
	GlobalCommandList->SetGraphicsRootSignature(BoundboxShaderRootSignature);

	Transform::Identity(TranslateMatrix);
	Transform::Identity(ScaleMatrix);

	Transform::Move(TranslateMatrix, sphere.Center.x, sphere.Center.y, sphere.Center.z);
	Transform::Scale(ScaleMatrix, Size * 0.27, Size * 0.27, Size * 0.27);

	BoundboxShader->RenderDefault(GlobalCommandList);
	camera.SetToDefaultMode();

	XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), XMLoadFloat4x4(&TranslateMatrix));

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(GlobalCommandList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &BoundboxColor, GAME_OBJECT_INDEX, 3, 16);

	SysRes.BoundingSphereMesh->Render(GlobalCommandList);
#endif
}