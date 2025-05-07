#pragma once
#include "GameObject.h"

class CrossHair : public GameObject {
private:
	LineBrush crosshair{};
	float recoil{};

	bool render_state{ true };

public:
	CrossHair();
	void InputRecoil(float Value);
	void Update(float Delta);
	void EnableRender();
	void DisableRender();
	void Render() override;
};