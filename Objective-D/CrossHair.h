#pragma once
#include "GameObject.h"

class CrossHair : public GameObject {
private:
	LineBrush crosshair{};
	float recoil{};

public:
	CrossHair();
	void InputRecoil(float Value);
	void Update(float Delta);
	void Render() override;
};