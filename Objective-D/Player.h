#pragma once
#include "GameObject.h"
#include "Scene.h"

class Player : public GameObject {
private:
	// ��ġ
	XMFLOAT3 position{ -130.0, 20.0, -130.0 };

	// ���� (�ٶ󺸴� ����)
	XMFLOAT3 rotation{};

	// �ͷ��� �浹 ó���� ���� �ͷ��� ��ƿ ��ü
	TerrainUtil terr{};

	// ī�޶� ������ ���� ���� ����ü
	Vector vec{};

	// �̵� ���� ��, ��, ��, ��
	bool move_front{}, move_back{}, move_right{}, move_left{};

	// �յ� �ӵ� �� �¿� �ӵ�
	float forward_speed{}, strafe_speed{};

	// �÷��̾ ��Ƽ踦 ���� �ִ� ����
	bool trigger_state{};

	// ���� �� �߻� ������ �ð�
	float current_fire_delay{};

	// �÷��̾� �ʰ��� �߻� ������ �ð�
	float over_fire_delay{};

	// �÷��̾� �� �߻� ����
	float dest_fire_delay{ 0.1 };


	// �÷��̾� �̵� �ӵ�
	float dest_move_speed{ 15.0 };

	// �ȱ� ��� ��鸲 ������Ʈ ��
	float walk_shake_num{};

	// �ȱ� ��� ��鸲 ��ġ ��
	float walk_shake_value{};

	// �ȱ� ��� ��鸲 ��� ��
	float walk_shake_result{};

public:
	Player();
	void InputMouseMotion(MotionEvent& Event) override;
	void InputMouse(MouseEvent& Event);
	void InputKey(KeyEvent& Event);
	void UpdateMoveSpeed(float FrameTime);
	void UpdateFire(float FrameTime);
	void UpdateCameraRotation();
	void UpdateTerrainCollision();
	void UpdateWalkMotion(float FrameTime);
	void Update(float FrameTime) override;
};