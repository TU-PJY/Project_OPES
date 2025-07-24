#pragma once
#include "Scene.h"

class Turret : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 headRotation{};
	xmfloat3 headRotationDest{};
	bool     createdByServer{};

	float flameSize{1.0};

	AABB     frustumBound{};
	BoundSphere lookRange{};
	OOBB     hitBox{};

	float    currentShootDelay{};
	float    flameRenderTime{};

	bool    inFrustum{};

	bool    targeted{};

	unsigned int currentTargetID{};

	GameObject* target{};

public:
	Turret(const xmfloat3& createPosition, float createRotation, bool createFromServer);
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};

