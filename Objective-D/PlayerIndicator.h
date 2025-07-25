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
	Text text2{};

	int turretCoolTime{};
	int beaconCoolTime{};
	int barrierCoolTime{};

	int type{};

	int currentIndex{};

public:
	PlayerIndicator(int chararcterType=CHARACTER_MG);
	void Render() override;
	void InputHP(int total, int current) override;
	void InputAmmo(int fullAmmo, int currentAmmo) override;
	void InputGrenade(int value) override;
	void InputTurretCoolTime(float time) override;
	void InputBeaconCoolTime(float time) override;
	void InputBarrierCoolTime(float time) override;
	void ScrollRight() override;
	void ScrollLeft() override;
	int GetCurrentIndex() override;
};