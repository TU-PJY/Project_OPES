#pragma once
#include "Scene.h"
#include "TextUtil.h"

class PlayerIndicator : public GameObject {
private:
	int currentHP{};
	int totalHP{};
	int currentAmmo{};
	int totalAmmo{};
	int grenCount{};
	Text text{};

public:
	PlayerIndicator();
	void Render() override;
	void InputHP(int total, int current) override;
	void InputAmmo(int fullAmmo, int currentAmmo) override;
	void InputGrenade(int value) override;
};