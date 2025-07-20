#pragma once
#include "GameObject.h"
#include "CollisionUtil.h"
#include "TerrainUtil.h"

class CenterBuilding : public GameObject {
private:
	XMFLOAT3  position{ -120.0, 0.0, -120.0 };
	OOBB      oobb{};
	int       totalHP{ 500 };
	int       currentHP{ 500 };
	GameObject* hpIndicator{};

public:
	CenterBuilding(float height_offset);
	~CenterBuilding();
	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
	XMFLOAT3 GetPosition() override;
	void GiveDamage(int Damage) override;
};