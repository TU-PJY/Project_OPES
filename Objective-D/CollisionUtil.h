#pragma once
#include "DirectX_3D.h"
#include "MeshUtil.h"
#include "Matrix.h"

class AABB;
class BoundSphere;

class OOBB {
private:
	bool Collide{};
	XMFLOAT3 BoundboxColor{ 1.0, 0.0, 0.0 };

public:
	BoundingOrientedBox oobb = BoundingOrientedBox();
	void UpdateAnimated(FBXMesh& Mesh, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix, int NodeIndex);
	void Update(Mesh* MeshPtr, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix, bool ApplySkinning=false);
	void Update(XMFLOAT3& Position, XMFLOAT3& Size, XMFLOAT3& Rotation);
	void Render();
	bool CheckCollision(const AABB& Other);
	bool CheckCollision(const BoundSphere& Other);
	bool CheckCollision(const OOBB& Other);
};


class AABB {
private:
	bool Collide{};
	XMFLOAT3 BoundboxColor{ 1.0, 0.0, 0.0 };

public:
	BoundingBox aabb = BoundingBox();
	void Update(XMFLOAT3& Position, XMFLOAT3& Size);
	void Render();
	bool CheckCollision(const AABB& Other);
	bool CheckCollision(const OOBB& Other);
	bool CheckCollision(const BoundSphere& Other);
};


class BoundSphere {
private:
	bool Collide{};
	float Size{};
	XMFLOAT3 BoundboxColor{ 1.0, 0.0, 0.0 };

public:
	BoundingSphere sphere = BoundingSphere();
	void Update(const XMFLOAT3& Center, float SizeValue);
	void Render();
	bool CheckCollision(const BoundSphere& Other);
	bool CheckCollision(const AABB& Other);
	bool CheckCollision(const OOBB& Other);
};