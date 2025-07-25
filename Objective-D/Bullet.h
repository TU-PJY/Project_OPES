#pragma once
#include "Scene.h"
#include "TerrainUtil.h"
#include "RayTargetUtil.h"

class Bullet : public GameObject {
private:
	TerrainUtil terrainUtil{};
	int         bulletDamage{};
	RayTarget   rayTarget{};

public:
	Bullet(int damage);
	~Bullet();
	Bullet() {};
	void updateTerrainCollision();
	void updateCollision();
	void Update(float Delta) override;
};