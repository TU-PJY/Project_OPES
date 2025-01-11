#pragma once
#include "DirectX_3D.h"
#include "MeshUtil.h"
#include "ShaderUtil.h"
#include "textureUtil.h"

class TerrainUtil {
public:
	XMFLOAT4X4 TerrainMatrix = Mat4::Identity();
	Mesh* TerrainMesh{};

	void InputData(XMFLOAT4X4& TMat, XMFLOAT4X4& RMat, XMFLOAT4X4& SMat, Mesh* MeshData);
	void InputPosition(XMFLOAT3& PositionValue, float HeightOffsetValue = 0.0);
	void SetHeightToTerrain(XMFLOAT3& PositionValue);
	bool CheckCollision(const TerrainUtil& Other);

private:
	XMFLOAT3 Position{};
	float HeightOffset{};
};
