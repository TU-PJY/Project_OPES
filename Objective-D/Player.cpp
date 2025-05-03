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
	// 총 발사 상태 활성화 / 비활성화
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
	// 각 움직임 변수에 대응된 키
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

	// 지형 충돌 처리
	//position.y -= 30.0 * FrameTime;
}

void Player::UpdateMoveSpeed(float FrameTime) {
	// 움직임 활성화 시 해당 방향으로 가속
	if (move_front && !move_back)
		forward_speed = std::lerp(forward_speed, dest_move_speed, 5.0 * FrameTime);
	if (move_back && !move_front)
		forward_speed = std::lerp(forward_speed, -dest_move_speed, 5.0 * FrameTime);
	if (move_right && !move_left)
		strafe_speed = std::lerp(strafe_speed, dest_move_speed, 5.0 * FrameTime);
	if (move_left && !move_right)
		strafe_speed = std::lerp(strafe_speed, -dest_move_speed, 5.0 * FrameTime);

	// 움직임 비활성화 또는 서로 반대 방향 이동 활성화 시 감속
	if ((!move_front && !move_back) || (move_front && move_back))
		forward_speed = std::lerp(forward_speed, 0.0, 5.0 * FrameTime);
	if ((!move_right && !move_left) || (move_right && move_left))
		strafe_speed = std::lerp(strafe_speed, 0.0, 5.0 * FrameTime);

	// 최종 이동
	Math::MoveForward(position, rotation.y, forward_speed * FrameTime);
	Math::MoveStrafe(position, rotation.y, strafe_speed * FrameTime);
}

void Player::UpdateFire(float FrameTime) {
	// 총 발사 간격을 업데이트 한다.
	// dest_fire_delay 간격으로 발사하게 된다.
	current_fire_delay -= FrameTime;
	if (current_fire_delay < 0.0)
		current_fire_delay = 0.0;

	// 발사 상태에서 current_fire_delay가 0.0이 되면 crosshair에 반동값 부여 -> 발사
	if (trigger_state) {
		if (current_fire_delay <= 0.0) {
			current_fire_delay = dest_fire_delay;
			if (auto crosshair = scene.Find("crosshair"); crosshair)
				crosshair->InputRecoil(0.1);
		}
	}
}

void Player::UpdateCameraRotation() {
	// 상하 카메라 회전 제한
	if (rotation.x < -90.0)
		rotation.x = -90.0;
	if (rotation.x > 90.0)
		rotation.x = 90.0;
	if (rotation.z < -90.0)
		rotation.z = -90.0;
	if (rotation.z > 90.0)
		rotation.z = 90.0;

	// 벡터 및 카메라 추적 업데이트
	Math::UpdateVector(vec, rotation);
	camera.Track(position, vec, 0);
}

void Player::UpdateTerrainCollision() {
	// 플레이어 높이가 항상 터레인 위에 위치하도록 한다
	if (auto terrain = scene.Find("map2"); terrain) {
		terr.InputPosition(position);
		terr.ClampToTerrain(terrain->GetTerrain(), position, 3.0);
	}
}