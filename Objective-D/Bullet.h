#pragma once
#include "Scene.h"
#include "TerrainUtil.h"
#include "RayTargetUtil.h"

class Bullet : public GameObject {
private:
	GameObject* currentTerrain{};
	TerrainUtil terrainUtil{};
	
	int         bulletDamage{};

	std::vector<OOBB> mapBounds{};

	RayTarget   rayTarget{};

public:
	Bullet(int damage);
	void updateTerrainCollision();
	void updateCollision();
	void Update(float Delta) override;
};