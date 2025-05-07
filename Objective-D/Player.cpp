#include "Player.h"
#include "Scene.h"
#include "MouseUtil.h"
#include "CameraController.h"
#include "ClampUtil.h"


// 생성자에서 입력받은 맵 오브젝트 이름으로 터레인 값을 받아온다.
Player::Player(std::string MapObjectName) {
	target_terrain_name = MapObjectName;
	mouse.StartMotionCapture(GlobalHWND);
	// 현재 맵에서 벽 oobb를 얻어온다.
	if (auto Map = scene.Find(MapObjectName); Map)
		MapOOBBData = Map->GetMapWallOOBB();
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
		// 마우스 모션 캡쳐 상태가 해제된 경우(윈도우 버튼 등으로 다른 윈도우에 포커싱된 경우)
		// 원래의 윈도우에 좌클릭으로 포커싱하면 모션 캡쳐 상태가 다시 활성화 된다.
		mouse.StartMotionCapture(GlobalHWND);
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
	// 총 발사 업데이트
	UpdateFire(FrameTime);

	// 이동 속도 가감속 업데이트
	UpdateMoveSpeed(FrameTime);

	// 카메라 회전 업데이트
	UpdateCameraRotation();

	// 걷기 모션 업데이트
	UpdateWalkMotion(FrameTime);

	// 터레인 충돌 처리 업데이트
	UpdateTerrainCollision(FrameTime);
}

void Player::Render() {
	//player_range.Render();
}

void Player::UpdateWalkMotion(float FrameTime) {
	walk_shake_num += FrameTime * 10.0;

	// 걷기 상태 활성화 시 카메라가 흔들리는 연출을 준다.
	if (move_front || move_back || move_right || move_left) 
		walk_shake_value = std::lerp(walk_shake_value, 1.5, FrameTime * 5.0);
	
	// 걷기 상태가 비활성화된 상태라면 점차 흔들림을 줄인다.
	else 
		walk_shake_value = std::lerp(walk_shake_value, 0.0, FrameTime * 5.0);

	// 최종 흔들림 값 계산
	walk_shake_result = sinf(walk_shake_num) * walk_shake_value;

	// 카메라 실제 회전
	camera.Rotate(rotation.x, rotation.y, rotation.z + walk_shake_result);
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

	// 플레이어 바운딩 스페어 업데이트
	player_sphere.Update(position, 0.6);

	// OOBB와 충돌을 체크하면서 이동
	Math::MoveWithSlide(position, rotation.y, forward_speed, strafe_speed, player_sphere, MapOOBBData, FrameTime);
}

void Player::UpdateFire(float FrameTime) {
	// 총 발사 간격을 업데이트 한다.
	// dest_fire_delay 간격으로 발사하게 된다.
	if (current_fire_delay > 0.0)
		current_fire_delay -= FrameTime;

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
	Clamp::ClampValue(rotation.x, -90.0, 90.0, CLAMP_FIX);
	Clamp::ClampValue(rotation.z, -90.0, 90.0, CLAMP_FIX);

	// 벡터 및 카메라 추적 업데이트
	Math::UpdateVector(vec, rotation);
	camera.Track(position, vec, 0);
}

void Player::UpdateTerrainCollision(float FrameTime) {

	// 플레이어 높이가 항상 터레인 위에 위치하도록 한다
	if (auto terrain = scene.Find(target_terrain_name); terrain) {
		terr.InputPosition(position);
		terr.ClampToTerrain(terrain->GetTerrain(), position, 3.0);
	}
}