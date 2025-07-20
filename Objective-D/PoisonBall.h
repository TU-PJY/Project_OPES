#pragma once
#include "Scene.h"
#include "MathUtil.h"
#include "TerrainUtil.h"

class PoisonBall : public GameObject {
private:
	XMFLOAT3    originPosition{};
	XMFLOAT3    position{};
	XMFLOAT3    destPosition{};
	Vector      vec{};
	BoundSphere bs{};
	bool        defenseModeState{};

	float       moveAngleX{};
	float       moveAngleY{};

	float       opacity{ 1.0 };
	XMFLOAT3    size{ 1.0, 1.0, 1.0 };
	bool        disappearState{};

	std::vector<OOBB> mapBoundData{};
	TerrainUtil       terrainUtil;
	GameObject*       currentTerrain{};

	// 面倒贸府侩 官款靛
	BoundSphere       poisonSphere{};

public:
	PoisonBall(const XMFLOAT3& createPosition, const XMFLOAT3& targetPosition, bool defenseMode=false);
	void updateCollision();
	void updateMove(float Delta);
	void updateDisappear(float Delta);
	void Update(float Delta) override;
	void Render() override;
};