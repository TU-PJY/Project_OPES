#pragma once
#include "Scene.h"

class Turret : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 headRotation{};
	bool     createdByServer{};

	AABB     frustumBound{};
	BoundSphere lookRange{};
	OOBB     hitBox{};

	bool    inFrustum{};

public:
	Turret(const xmfloat3& createPosition, float createRotation, bool createFromServer);
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};

