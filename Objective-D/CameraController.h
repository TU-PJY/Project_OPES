#pragma once
#include "GameObject.h"
#include "CameraUtil.h"
#include "MouseUtil.h"

// 관전 카메라 컨트롤러
class CameraController : public GameObject {
private:
	bool MoveForward{}, MoveBackward{}, MoveRight{}, MoveLeft{};
	bool MoveUp{}, MoveDown{};

	XMFLOAT3 CamPosition{ 0.0, 80.0, 0.0 };

	// 카메라 회전
	XMFLOAT3 CamRotation{};
	XMFLOAT3 DestCamRotation{};

public:
	CameraController() {
		camera.Move(CamPosition);
	}

	void InputKey(KeyEvent& Event) override {
		switch (Event.Type) {
		case WM_KEYDOWN:
			switch (Event.Key) {
			case 'W': MoveForward = true; break;
			case 'A': MoveLeft = true; break;
			case 'S': MoveBackward = true; break;
			case 'D': MoveRight = true; break;
			case VK_SPACE: MoveUp = true; break;
			case VK_CONTROL: MoveDown = true; break;
			}
			break;

		case WM_KEYUP:
			switch (Event.Key) {
			case 'W': MoveForward = false; break;
			case 'A': MoveLeft = false; break;
			case 'S': MoveBackward = false; break;
			case 'D': MoveRight = false; break;
			case VK_SPACE: MoveUp = false; break;
			case VK_CONTROL: MoveDown = false; break;
			}
			break;
		}
	}

	void InputMouseMotion(MotionEvent& Event) override {
		if (GetCapture() == Event.CaptureState) {
			mouse.HideCursor();
			GetCapture();

			XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, 0.3);
			UpdateMotionRotation(CamRotation, Delta.x, Delta.y);
		}
	}

	void InputMouse(MouseEvent& Event) override {
		switch (Event.Type) {
		case WM_LBUTTONDOWN:
			mouse.StartMotionCapture(Event.hWnd);
			break;

		case WM_LBUTTONUP:
			mouse.EndMotionCapture();
			break;
		}
	}

	void Update(float FT) override {
		// 상하 회전반경 제한
		if (CamRotation.x > 90.0)
			CamRotation.x = 90.0;
		else if (CamRotation.x < -90.0)
			CamRotation.x = -90.0;

		if (MoveForward)
			camera.MoveForward(FT * 40);
		if (MoveBackward)
			camera.MoveForward(-FT * 40);
		if (MoveRight)
			camera.MoveStrafe(FT * 40);
		if (MoveLeft)
			camera.MoveStrafe(-FT * 40);
		if (MoveUp)
			camera.MoveUp(FT * 40);
		if (MoveDown)
			camera.MoveUp(-FT * 40);

		camera.Rotate(CamRotation.x, CamRotation.y, 0.0);
		CamPosition = camera.GetPosition();
	}
};