#pragma once
#include "Scene.h"

class Beacon : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	BoundSphere healZone{};
	OOBB     frustumBound{};

	float currentDelay{};
	float totalHP{ BEACON_DURABILITY };
	float currentHP{ BEACON_DURABILITY };

	bool inFrustum{};

	GameObject* hpInd{};
	
public:
	Beacon(const xmfloat3& createPosition, float rotation, bool createFromServer=false);
	~Beacon();
	void Update(float Delta) override;
	void Render() override;
};

