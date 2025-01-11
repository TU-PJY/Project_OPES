#pragma once
#include "DirectX_3D.h"
#include "MeshUtil.h"

class AABB;
class Range;

class OOBB {
private:
	bool Collide{};

	XMFLOAT4X4 TranslateMatrix{};
	XMFLOAT4X4 RotateMatrix{};
	XMFLOAT4X4 ScaleMatrix{};

	XMFLOAT3 BoundboxColor{ 1.0, 0.0, 0.0 };

public:
	BoundingOrientedBox oobb = BoundingOrientedBox();
	void Update(Mesh* MeshPtr, XMFLOAT4X4& TMatrix, XMFLOAT4X4& RMatrix, XMFLOAT4X4& SMatrix);
	void Update(XMFLOAT3& Position, XMFLOAT3& Size, XMFLOAT3& Rotation);
	void Render();
	bool CheckCollision(const AABB& Other);
	bool CheckCollision(const Range& Other);
	bool CheckCollision(const OOBB& Other);
};


class AABB {
private:
	bool Collide{};

	XMFLOAT4X4 TranslateMatrix{};
	XMFLOAT4X4 ScaleMatrix{};

	XMFLOAT3 BoundboxColor{ 1.0, 0.0, 0.0 };

public:
	BoundingBox aabb = BoundingBox();
	void Update(XMFLOAT3& Position, XMFLOAT3& Size);
	void Render();
	bool CheckCollision(const AABB& Other);
	bool CheckCollision(const OOBB& Other);
	bool CheckCollision(const Range& Other);
};


class Range {
private:
	bool Collide{};

	float Size{};

	XMFLOAT4X4 TranslateMatrix{};
	XMFLOAT4X4 ScaleMatrix{};

	XMFLOAT3 BoundboxColor{ 1.0, 0.0, 0.0 };

public:
	BoundingSphere sphere = BoundingSphere();
	void Update(const XMFLOAT3& Center, float SizeValue);
	void Render();
	bool CheckCollision(const Range& Other);
	bool CheckCollision(const AABB& Other);
	bool CheckCollision(const OOBB& Other);
};