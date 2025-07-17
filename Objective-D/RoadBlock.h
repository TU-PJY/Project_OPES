#pragma once
#include "Scene.h"

class RoadBlock : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	XMFLOAT3 size{ 8.0, 8.0, 8.0 };
	OOBB     oobb{};
	int      numBlock{};

	bool     disappearState{};

public:
	RoadBlock(const XMFLOAT3& createPosition, float degrees, int num);
	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
};