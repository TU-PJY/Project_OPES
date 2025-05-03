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


	// �÷��̾ ��Ƽ踦 ���� �ִ� ����
	bool trigger_state{};

	// ���� �� �߻� ������ �ð�
	float current_fire_delay{};

	// �÷��̾� �� �߻� ����
	float dest_fire_delay{ 0.1 };

	// �÷��̾� �̵� �ӵ�
	float dest_move_speed{ 15.0 };

public:
	Player();
	void InputMouseMotion(MotionEvent& Event) override;
	void InputMouse(MouseEvent& Event);
	void InputKey(KeyEvent& Event);
	void Update(float FrameTime) override;
};