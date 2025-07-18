#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Bullet : public GameObject {
private:
	Vector      vec{};
	XMFLOAT3    position{};
	XMFLOAT3    destination{};
	XMFLOAT3    rotation{};
	XMFLOAT3    size{ 1.0, 1.0, 1.0 };

	BoundSphere bs{};

	GameObject* currentTerrain{};
	TerrainUtil terrainUtil{};

public:
	Bullet(const XMFLOAT3& createPosition, const XMFLOAT3& targetPosition, const std::string& );
	void updateTerrainCollision();
	void Update(float Delta) override;
	void Render() override;
};