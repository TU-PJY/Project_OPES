#pragma once
#include "GameObject.h"

class HP_Indicator : public GameObject {
private:
	int full_hp{};
	int current_hp{};
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	float renderSize{ 1.0 };
	Vector vec{};
	float length{};

public:
	HP_Indicator();
	void SetSize(float Value);
	void InputPosition(XMFLOAT3& inputPos, float heightOffset);
	void InputHP(int fullHP, int currentHP);
	void Render();
};