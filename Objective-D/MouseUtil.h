#pragma once
#include "DirectX_3D.h"

// 마우스를 담당하는 유틸이다. 

enum class Type
{LEFT_BUTTON, RIGHT_BUTTON};

enum class State
{BUTTON_DOWN, BUTTON_UP};

class MouseUtil {
public:
	float x{}, y{};
	POINT MotionPosition{};
	POINT ClientPosition{};
	bool LBUTTONDOWN{}, RBUTTONDOWN{};

	void HideCursor();
	void StartMotionCapture(HWND hWnd);
	void EndMotionCapture();
	void UpdateMotionPosition(POINT& MotionPosition);
	XMFLOAT2 GetMotionDelta(POINT& MotionPosition, float Sensivity);
	POINT CurrentPosition();
	void UpdateMousePosition(HWND hWnd);
};
extern MouseUtil mouse;
