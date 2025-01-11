#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include <cmath>
#include "PickingUtil.h"

class TestObject : public GameObject {
public:
	XMFLOAT3 Position{0.0, 0.0, 0.0};
	XMFLOAT3 Rotation{};
	XMFLOAT3 RotationDest{};
	XMFLOAT3 Size{ 0.5, 0.5, 0.5 };
	XMFLOAT3 Color{ 0.0, 0.0, 0.0 };

	LineBrush line;

	OOBB oobb;
	
	float point = 0.1;

	TestObject() {
		line.SetColor(1.0, 1.0, 1.0);
		Position.z = 5.0;
	}

	void InputMouseMotion(MotionEvent& Event) {
		if (GetCapture() == Event.CaptureState) {
			mouse.HideCursor();
			GetCapture();

			XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, 0.5);
			UpdateMotionRotation(RotationDest, Delta.x, Delta.y);
		}
	}

	void InputMouse(MouseEvent& Event) {
		switch (Event.Type) {
		case WM_LBUTTONDOWN:
			mouse.StartMotionCapture(Event.hWnd);
			break;

		case WM_LBUTTONUP:
			mouse.EndMotionCapture();
			break;

		case WM_RBUTTONDOWN:
			if (PickingUtil::PickByViewportOOBB(mouse.x, mouse.y, oobb)) {
				Color = XMFLOAT3(1.0, 0.0, 0.0);
			}
			else
				Color = XMFLOAT3(0.0, 0.0, 0.0);
			break;
		}
	}

	void Update(float FT) {
		Rotation.x = std::lerp(Rotation.x, RotationDest.x, FT * 10);
		Rotation.y = std::lerp(Rotation.y, RotationDest.y, FT * 10);
	}

	void Render() override {
		camera.Move(XMFLOAT3(0.0, 0.0, -3.0));
		InitRenderState();
		Transform::Move(TranslateMatrix, 0.0, -1.0, 0.0);
		Transform::Rotate(RotateMatrix, Rotation.x, Rotation.y, 0.0);
		Render3D(HelicopterMesh, HelicopterTex);
	}
};