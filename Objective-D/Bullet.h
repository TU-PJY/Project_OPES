#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Bullet : public GameObject {
private:
	Vector      vec{};
	XMFLOAT3    position{};
	XMFLOAT3    startPosition{};
	float       degreesY{};
	float       degreesX{};

	BoundSphere bulletBound{};

	GameObject* currentTerrain{};
	TerrainUtil terrainUtil{};
	std::string currentMapName{};

	float       bulletOpacity{};
	
	int         bulletDamage{};

	std::vector<OOBB> mapOOBBData{};

public:
	Bullet(const XMFLOAT3& createPosition, float degreesY, float degreesX, int damage, const std::string& terrainName);
	void updateTerrainCollision();
	void updateCollision();
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};