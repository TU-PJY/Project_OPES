#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Grenade : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 velocity{};
	XMFLOAT3 rotation{};

	// Æ¨±è °è¼ö
	float    restitution{ 0.65f };

	// ¼öÆò °¨¼è
	float    friction{ 0.5f };   
	float    gravity{ 9.8f };
	bool     isStopped{ false };

	GameObject* currentTerrain{};
	TerrainUtil terrainUtil{};
	std::vector<OOBB> mapBounds{};

	AABB        grenadeBound{};

public:
	Grenade(const XMFLOAT3& createPosition, const XMFLOAT3& rotation);
	XMVECTOR getNormalFromAngle(const XMFLOAT3& angleDeg);
	XMFLOAT3 getDirectionFromRotation(const XMFLOAT3& angleDeg);
	XMFLOAT3 getEulerFromVelocity(const XMFLOAT3& velocity);
	void updateMove(float Delta);
	void updateBound();
	void updateCollision();
	void Update(float Delta) override;
	void Render() override;
};

