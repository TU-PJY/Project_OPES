#pragma once
#include "Scene.h"

class Turret : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 headRotation{};
	xmfloat3 headRotationDest{};
	bool     createdByServer{};

	float    heightOffset{1.0};

	AABB     frustumBound{};
	BoundSphere lookRange{};
	OOBB     hitBox{};

	// 20초후 스스로 파괴된다.
	float    currentHP{0};

	float    currentShootDelay{};
	float    flameRenderTime{};

	bool    inFrustum{};
	bool    targeted{};
	bool    prevTargeted{};

	bool    operationStart{};
	bool    destroyState{};
	float   flyAcc{};
	xmfloat3 rotationOffset{};
	float   destroyTime{};
	float   currentSmokeFrame{};

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

