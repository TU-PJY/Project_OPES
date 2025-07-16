#pragma once
#include "Scene.h"
#include "MathUtil.h"

class PoisonBall : public GameObject {
private:
	XMFLOAT3    originPosition{};
	XMFLOAT3    position{};
	XMFLOAT3    destPosition{};
	Vector      vec{};
	BoundSphere bs{};
	bool        defenseModeState{};

	float       opacity{ 1.0 };
	XMFLOAT3    size{ 1.0, 1.0, 1.0 };
	bool        disapperState{};

public:
	PoisonBall(const XMFLOAT3& createPosition, const XMFLOAT3& targetPosition, bool defenseMode=false);
	void UpdateMove(float Delta);
	void updateDisappear(float Delta);
	void Update(float Delta) override;
	void Render() override;
};