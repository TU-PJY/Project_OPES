#include "TerrainUtil.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"
#include "MathUtil.h"
#include "CameraUtil.h"

// 터레인 충돌처리 유틸이다.

// 터레인의 정보를 입력한다.
// 한 터레인 매쉬 당 한 번만 실행하면 된다. 생성자에서 실행할 것을 권장한다.
// 터레인이 아닌 객체는 해당 함수를 실행할 필요가 없다.
void TerrainUtil::InputData(XMFLOAT4X4& TMat, XMFLOAT4X4& RMat, XMFLOAT4X4& SMat, Mesh* MeshData) {
	XMMATRIX world = XMMatrixMultiply(
		XMMatrixMultiply(XMLoadFloat4x4(&SMat),
		XMLoadFloat4x4(&RMat)),
		XMLoadFloat4x4(&TMat)
	);

	XMStoreFloat4x4(&TerrainMatrix, world);

	TerrainMesh = MeshData;

	// 높이 캐시를 비우고 새로운 값으로 높이 캐시를 설정한다.
	TerrainMesh->ClearHeightCache();
	TerrainMesh->SetHeightCache(TerrainMesh, TerrainMatrix);
}

void TerrainUtil::AddData(XMFLOAT4X4& TMat, XMFLOAT4X4& RMat, XMFLOAT4X4& SMat, Mesh* MeshData) {
	XMFLOAT4X4 Matrix{};
	std::vector<xmfloat3> NewData{};

	XMMATRIX world = XMMatrixMultiply(
		XMMatrixMultiply(XMLoadFloat4x4(&SMat),
			XMLoadFloat4x4(&RMat)),
		XMLoadFloat4x4(&TMat)
	);

	XMStoreFloat4x4(&Matrix, world);

	XMMATRIX gmtxWorld = XMLoadFloat4x4(&Matrix);

	for (UINT i = 0; i < MeshData->Indices; ++i) {
		XMFLOAT3 v = MeshData->Position[MeshData->PnIndices[i]];
		XMVECTOR vWorld = XMVector3Transform(XMLoadFloat3(&v), gmtxWorld);
		XMFLOAT3 worldVertex;
		XMStoreFloat3(&worldVertex, vWorld);
		NewData.push_back(worldVertex);
	}

	std::cout << MeshData->Indices << " " << NewData.size() << std::endl;

	for (size_t i = 0; i < NewData.size(); i += 3) {
		XMFLOAT3& v0 = NewData[i];
		XMFLOAT3& v1 = NewData[i + 1];
		XMFLOAT3& v2 = NewData[i + 2];

		XMVECTOR a = XMLoadFloat3(&v0);
		XMVECTOR b = XMLoadFloat3(&v1);
		XMVECTOR c = XMLoadFloat3(&v2);

		XMVECTOR ab = b - a;
		XMVECTOR ac = c - a;

		XMVECTOR normal = XMVector3Cross(ab, ac);
		if (XMVectorGetY(normal) < 0.0f) {
			std::swap(v1, v2); // 법선 방향 반전
		}
	}

	TerrainMesh->HeightCache.insert(TerrainMesh->HeightCache.end(), NewData.begin(), NewData.end());
}

// 아래의 함수들은 터레인 객체에서 실행 할 필요 없다.

// 현재 위치와 높이 오프셋을 입력한다
void TerrainUtil::InputPosition(XMFLOAT3& PositionValue, float HeightOffsetValue) {
	Position = PositionValue;
	HeightOffset = HeightOffsetValue;
}

// 터레인과 충돌 시 대상 높이를 터레인 높이로 변경한다.

// 대상 높이를 터레인 높이에 고정시킨다.
void TerrainUtil::ClampToTerrain(const TerrainUtil& Other, XMFLOAT3& PositionValue, float HeightOffsetValue) {
	PositionValue.y = Other.TerrainMesh->GetHeightAtPosition(Position.x, Position.z) + HeightOffsetValue;
}

void TerrainUtil::SetHeightToTerrain(XMFLOAT3& PositionValue) {
	PositionValue.y = Position.y + HeightOffset;
}

// 입력한 높이가 터레인의 바닥 높이보다 낮은지 검사한다. 낮을 경우 true를 리턴한다.
bool TerrainUtil::CheckCollision(const TerrainUtil& Other) {
	float Height = Other.TerrainMesh->GetHeightAtPosition(Position.x, Position.z) + HeightOffset;

	if (Position.y < Height) {
		Position.y = Height;
		return true;
	}

	return false;
}

// 현재 지점에서 터레인의 각도를 계산한다.
XMFLOAT3 TerrainUtil::GetNormalAtPoint(const TerrainUtil& Other) {
	XMFLOAT3 Normal = Other.TerrainMesh->GetNormalAtPosition(Position.x, Position.z);
	return Normal;
}

// 현재 지점에서 터레인의 각도를 계산한다.
XMFLOAT3 TerrainUtil::GetAngleAtPoint(const TerrainUtil& Other) {
	XMFLOAT3 Angle = Other.TerrainMesh->GetAngleAtPosition(Position.x, Position.z);
	return Angle;
}

XMFLOAT3 TerrainUtil::CheckCollisionRay(const TerrainUtil& Other, float& Distance) {
	XMFLOAT3 ReturnValue{};
	Other.TerrainMesh->PickTerrainFromCamera(camera.GetViewMatrix(), ReturnValue, Distance);
	return ReturnValue;
}

XMFLOAT3 TerrainUtil::CheckCollisionRay(const TerrainUtil& Other, XMVECTOR& Origin, XMVECTOR& Direction, float& Distance) {
	if (!Other.TerrainMesh)
		return xmfloat3(0.0, 0.0, 0.0);
	XMFLOAT3 ReturnValue{};
	Other.TerrainMesh->PickTerrainFromRay(Origin, Direction, ReturnValue, Distance);
	return ReturnValue;
}

void TerrainUtil::ExportTerrainData(const std::string& OutFileName) {
	std::ofstream Out{ OutFileName };
	if (!Out.is_open())
		return;

	if (!TerrainMesh->HeightCache.empty()) {
		for (auto const& P : TerrainMesh->HeightCache) {
			XMFLOAT3 OutData = P;

			Out << "<point x=\"" << P.x
				<< "\" y=\"" << P.y
				<< "\" z=\"" << P.z
				<< "\" />\n";
		}
	}

	Out.close();
}