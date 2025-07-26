#pragma once
#include "Scene.h"
#include "TerrainUtil.h"
#include "RayTargetUtil.h"

class SpreadBullet : public GameObject {
private:
	std::vector<std::pair<XMVECTOR, XMVECTOR>> rays{};
	std::vector<RayTarget> targets{};
	std::map<GameObject*, int> damageList{};
	TerrainUtil terrainUtil{};
	int damage{};

public:
	SpreadBullet(int damage);
	void Update(float Delta) override;
};

