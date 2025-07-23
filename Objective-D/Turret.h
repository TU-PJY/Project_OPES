#pragma once
#include "Scene.h"

class Turret : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 headRotation{};
	xmfloat3 headRotationDest{};
	bool     createdByServer{};

	AABB     frustumBound{};
	BoundSphere lookRange{};
	OOBB     hitBox{};

	bool    inFrustum{};

	bool    targeted{};

	GameObject* target{};

public:
	Turret(const xmfloat3& createPosition, float createRotation, bool createFromServer);
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};

