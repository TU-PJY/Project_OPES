#pragma once
#include "GameObject.h"
#include "Scene.h"

class Player : public GameObject {
private:
	XMFLOAT3 Position{ -130.0, 20.0, -130.0 };
	XMFLOAT3 Rotation{};

	TerrainUtil terr{};

	Vector vec{};

	bool move_front{}, move_back{}, move_right{}, move_left{};
	float forward_speed{}, strafe_speed{};

public:
	void InputMouseMotion(MotionEvent& Event) override {
		if (GetCapture() == Event.CaptureState) {
			mouse.HideCursor();
			GetCapture();

			XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, 0.1);
			UpdateMotionRotation(Rotation, Delta.x, Delta.y);
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

	void InputKey(KeyEvent& Event) {
		switch (Event.Type) {
		case WM_KEYDOWN:
			switch (Event.Key) {
			case 'W':
				move_front = true; break;
			case 'S':
				move_back = true; break;
			case 'A':
				move_left = true; break;
			case 'D':
				move_right = true; break;
			}
			break;

		case WM_KEYUP:
			switch (Event.Key) {
			case 'W':
				move_front = false; break;
			case 'S':
				move_back = false; break;
			case 'A':
				move_left = false; break;
			case 'D':
				move_right = false; break;
			}
			break;
		}
	}

	void Update(float FrameTime) override {
		if (move_front)
			Math::MoveForward(Position, Rotation.y, 20.0 * FrameTime);
		if(move_back)
			Math::MoveForward(Position, Rotation.y, -20.0 * FrameTime);
		if(move_left)
			Math::MoveStrafe(Position, Rotation.y, -20.0 * FrameTime);
		if (move_right)
			Math::MoveStrafe(Position, Rotation.y, 20.0 * FrameTime);
		
		Position.y -= 10.0 * FrameTime;
		Math::UpdateVector(vec, Rotation);
		camera.Track(Position, vec, 0);

		if (auto terrain = scene.Find("map2"); terrain) {
			terr.InputPosition(Position, 3.0);
			if (terr.CheckCollision(terrain->GetTerrain()))
				terr.SetHeightToTerrain(Position);
		}
	}
};