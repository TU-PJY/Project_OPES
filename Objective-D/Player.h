#pragma once
#include "GameObject.h"
#include "Scene.h"

class Player : public GameObject {
private:
	XMFLOAT3 position{ -130.0, 20.0, -130.0 };
	XMFLOAT3 rotation{};

	TerrainUtil terr{};
	Vector vec{};

	bool move_front{}, move_back{}, move_right{}, move_left{};
	float forward_speed{}, strafe_speed{};


	// 플레이어가 방아쇠를 당기고 있는 상태
	bool trigger_state{};

	// 현재 총 발사 딜레이 시간
	float current_fire_delay{};

	// 플레이어 총 발사 간격
	float dest_fire_delay{ 0.1 };

	// 플레이어 이동 속도
	float dest_move_speed{ 15.0 };

public:
	Player();
	void InputMouseMotion(MotionEvent& Event) override;
	void InputMouse(MouseEvent& Event);
	void InputKey(KeyEvent& Event);
	void Update(float FrameTime) override;
};