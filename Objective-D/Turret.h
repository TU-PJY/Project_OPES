#pragma once
#include "Scene.h"

class Turret : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 headRotation{};
	xmfloat3 headRotationDest{};
	bool     createdByServer{};

	float    heightOffset{0.0};

	AABB     frustumBound{};
	BoundSphere lookRange{};
	OOBB     hitBox{};

	// 20초후 스스로 파괴된다.
	float    currentHP{20};

	float    currentShootDelay{};
	float    flameRenderTime{};

	bool    inFrustum{};
	bool    targeted{};

	unsigned int currentTargetID{};

	GameObject* target{};
	GameObject* hpInd{};

public:
	Turret(const xmfloat3& createPosition, float createRotation, bool createFromServer);
	~Turret();
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};

