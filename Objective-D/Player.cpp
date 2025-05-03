#include "Player.h"
#include "Scene.h"
#include "MouseUtil.h"
#include "CameraController.h"


Player::Player() {
	mouse.StartMotionCapture(GlobalHWND);
}

void Player::InputMouseMotion(MotionEvent& Event) {
	if (GetCapture() == Event.CaptureState) {
		mouse.HideCursor();
		GetCapture();

		XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, 0.08);
		UpdateMotionRotation(rotation, Delta.x, Delta.y);
	}
}

void Player::InputMouse(MouseEvent& Event) {
	// �� �߻� ���� Ȱ��ȭ / ��Ȱ��ȭ
	switch (Event.Type) {
	case WM_LBUTTONDOWN:
		trigger_state = true;
		break;

	case WM_LBUTTONUP:
		trigger_state = false;
		break;
	}
}

void Player::InputKey(KeyEvent& Event) {
	// �� ������ ������ ������ Ű
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'W', move_front);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'S', move_back);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'A', move_left);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'D', move_right);

	if (Event.Type == WM_KEYDOWN && Event.Key == VK_ESCAPE)
		scene.Exit();
}

void Player::Update(float FrameTime) {
	UpdateFire(FrameTime);
	UpdateMoveSpeed(FrameTime);
	UpdateCameraRotation();
	UpdateTerrainCollision();

	// ���� �浹 ó��
	//position.y -= 30.0 * FrameTime;
}

void Player::UpdateMoveSpeed(float FrameTime) {
	// ������ Ȱ��ȭ �� �ش� �������� ����
	if (move_front && !move_back)
		forward_speed = std::lerp(forward_speed, dest_move_speed, 5.0 * FrameTime);
	if (move_back && !move_front)
		forward_speed = std::lerp(forward_speed, -dest_move_speed, 5.0 * FrameTime);
	if (move_right && !move_left)
		strafe_speed = std::lerp(strafe_speed, dest_move_speed, 5.0 * FrameTime);
	if (move_left && !move_right)
		strafe_speed = std::lerp(strafe_speed, -dest_move_speed, 5.0 * FrameTime);

	// ������ ��Ȱ��ȭ �Ǵ� ���� �ݴ� ���� �̵� Ȱ��ȭ �� ����
	if ((!move_front && !move_back) || (move_front && move_back))
		forward_speed = std::lerp(forward_speed, 0.0, 5.0 * FrameTime);
	if ((!move_right && !move_left) || (move_right && move_left))
		strafe_speed = std::lerp(strafe_speed, 0.0, 5.0 * FrameTime);

	// ���� �̵�
	Math::MoveForward(position, rotation.y, forward_speed * FrameTime);
	Math::MoveStrafe(position, rotation.y, strafe_speed * FrameTime);
}

void Player::UpdateFire(float FrameTime) {
	// �� �߻� ������ ������Ʈ �Ѵ�.
	// dest_fire_delay �������� �߻��ϰ� �ȴ�.
	current_fire_delay -= FrameTime;
	if (current_fire_delay < 0.0)
		current_fire_delay = 0.0;

	// �߻� ���¿��� current_fire_delay�� 0.0�� �Ǹ� crosshair�� �ݵ��� �ο� -> �߻�
	if (trigger_state) {
		if (current_fire_delay <= 0.0) {
			current_fire_delay = dest_fire_delay;
			if (auto crosshair = scene.Find("crosshair"); crosshair)
				crosshair->InputRecoil(0.1);
		}
	}
}

void Player::UpdateCameraRotation() {
	// ���� ī�޶� ȸ�� ����
	if (rotation.x < -90.0)
		rotation.x = -90.0;
	if (rotation.x > 90.0)
		rotation.x = 90.0;
	if (rotation.z < -90.0)
		rotation.z = -90.0;
	if (rotation.z > 90.0)
		rotation.z = 90.0;

	// ���� �� ī�޶� ���� ������Ʈ
	Math::UpdateVector(vec, rotation);
	camera.Track(position, vec, 0);
}

void Player::UpdateTerrainCollision() {
	// �÷��̾� ���̰� �׻� �ͷ��� ���� ��ġ�ϵ��� �Ѵ�
	if (auto terrain = scene.Find("map2"); terrain) {
		terr.InputPosition(position);
		terr.ClampToTerrain(terrain->GetTerrain(), position, 3.0);
	}
}