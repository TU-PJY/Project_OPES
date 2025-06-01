#include "Player.h"
#include "Scene.h"
#include "MouseUtil.h"
#include "CameraController.h"
#include "ClampUtil.h"

std::default_random_engine rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(0, 1);

void SendMovePacket(float x, float y,float z);
void SendViewingAnglePacket(float x, float y, float z);
void SendAnimaionPacket(unsigned short playerState);
// 생성자에서 입력받은 맵 오브젝트 이름으로 터레인 값을 받아온다.
Player::Player(std::string MapObjectName) {
	target_terrain_name = MapObjectName;
	mouse.StartMotionCapture(GlobalHWND);

	// 현재 맵에서 벽 oobb를 얻어온다.
	if (auto Map = scene.Find(MapObjectName); Map)
		map_oobb_data = Map->GetMapWallOOBB();

	if (auto building = scene.Find("center_building"); building)
		map_oobb_data.emplace_back(building->GetOOBB());

	// 오버헤드 감소를 위해 미리 크로스헤어 오브젝트 포인터를 저장한다.
	if (auto Object = scene.Find("crosshair"))
		crosshair_ptr = Object;
}

void Player::InputMouseMotion(MotionEvent& Event) {
	if (GetCapture() == Event.CaptureState) {
		mouse.HideCursor();
		GetCapture();

		// 정조준 시 감도를 절반으로 낮춘다
		float sensivity = 0.08;
		if (gun_zoomed)  sensivity = 0.04;
		XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, sensivity);
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

	case WM_RBUTTONDOWN:
		gun_zoomed = true;
		fov_dest = -20.0;
		crosshair_ptr->DisableRender();
		break;

	case WM_RBUTTONUP:
		gun_zoomed = false;
		fov_dest = 0.0;
		crosshair_ptr->EnableRender();
		break;
	}
}

void Player::InputKey(KeyEvent& Event) {
	// 각 움직임 변수에 대응된 키
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'W', move_front);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'S', move_back);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'A', move_left);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'D', move_right);
}

// 서버 부하를 방지하기 위해 0.05초 간격으로 패킷 전송 
void Player::SendPacket(float Delta) {
	send_delay += Delta;

	if (send_delay >= 0.025) {
		if (send_order == 1) {
			if (player_state == STATE_MOVE || player_state == STATE_MOVE_SHOOT)
				SendMovePacket(position.x, position.y - 3.0, position.z);
		}

		else if (send_order == 2) {
			if (old_rotation.x != rotation.x || old_rotation.y != rotation.y || old_rotation.z != rotation.z) {
				SendViewingAnglePacket(rotation.x, rotation.y, rotation.z);
				old_rotation = rotation;
			}
		}

		else if (send_order == 3)
			SendAnimaionPacket(current_state);

		send_order += 1;
		if (send_order > 3)
			send_order = 1;

		float over_time = 0.025 - send_delay;
		send_delay = over_time;
	}
}

void Player::Update(float FrameTime) {
	// 총 발사 업데이트
	UpdateFire(FrameTime);

	// 총 업데이트
	UpdateGun(FrameTime);

	// 이동 속도 가감속 업데이트
	UpdateMoveSpeed(FrameTime);
	 
	// 카메라 업데이트
	UpdateCamera(FrameTime);

	// 터레인 충돌 처리 업데이트
	UpdateTerrainCollision(FrameTime);

	// 총 - 맵 오브젝트 충돌 처리 업데이트
	UpdateGunCollision();

	// 서버로 패킷 전송
	SendPacket(FrameTime);
}

void Player::Render() {
	// 1인칭 총 렌더링
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, gun_rotation.x, gun_rotation.y + gun_rotation_offset, gun_rotation.z);
	Transform::Move(TranslateMatrix, gun_position_offset.x, gun_position_offset.y, gun_position_offset.z + gun_offset);
	Render3D(MESH.machine_gun, TEX.scifi);

	// 레드 도트 렌더링
	SetLightUse(DISABLE_LIGHT);
	Render3D(MESH.dot_machine_gun, TEX.scifi);

	// flame_time 동안 불꽃 렌더링
	if (flame_time > 0.0) {
		// 불꽃 렌더링
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
	}

	gun_oobb.Update(MESH.machine_gun, TranslateMatrix, RotateMatrix, ScaleMatrix, true);
	gun_oobb.Render();
}

void Player::UpdateMoveSpeed(float FrameTime) {
	// 움직임 활성화 시 해당 방향으로 가속
	if (move_front && !move_back)
		forward_speed = std::lerp(forward_speed, dest_move_speed, 10.0 * FrameTime);
	if (move_back && !move_front)
		forward_speed = std::lerp(forward_speed, -dest_move_speed, 10.0 * FrameTime);
	if (move_right && !move_left)
		strafe_speed = std::lerp(strafe_speed, dest_move_speed, 10.0 * FrameTime);
	if (move_left && !move_right)
		strafe_speed = std::lerp(strafe_speed, -dest_move_speed, 10.0 * FrameTime);

	// 움직임 비활성화 또는 서로 반대 방향 이동 활성화 시 감속
	if ((!move_front && !move_back) || (move_front && move_back)) 
		forward_speed = std::lerp(forward_speed, 0.0, 10.0 * FrameTime);
	
	if ((!move_right && !move_left) || (move_right && move_left)) 
		strafe_speed = std::lerp(strafe_speed, 0.0, 10.0 * FrameTime);

	// 플레이어 바운딩 스페어 업데이트
	player_sphere.Update(position, 2.0);

	// OOBB와 충돌을 체크하면서 이동
	Math::MoveWithSlide(position, rotation.y, forward_speed, strafe_speed, player_sphere, map_oobb_data, FrameTime);

	if ((move_front && !move_back) || (move_back && !move_front) || 
		(move_right && !move_left) || (move_left && !move_right)) {
		if (trigger_state)
			player_state = STATE_MOVE_SHOOT;
		else
			player_state = STATE_MOVE;
	}
	else {
		if (trigger_state)
			player_state = STATE_IDLE_SHOOT;
		else
			player_state = STATE_IDLE;
	}

	current_state = player_state;
}

void Player::UpdateFire(float FrameTime) {
	// 총 발사 간격을 업데이트 한다.
	// dest_fire_delay 간격으로 발사하게 된다.
	if (current_fire_delay > 0.0)
		current_fire_delay -= FrameTime;

	if (flame_time > 0.0)
		flame_time -= FrameTime;

	// 발사 상태에서 current_fire_delay가 0.0이 되면 crosshair에 반동값 부여 -> 발사
	if (trigger_state) {
		if (current_fire_delay <= 0.0) {

			// LAYER1에 존재하는 모든 몬스터들에 대한 피격 검사
			// 관통이 아닌 총의 경우 한 마리라도 피격이 발생하면 검사 조기 종료
			size_t size = scene.LayerSize(LAYER1);
			for (int i = 0; i < size; i++) {
				if (auto target = scene.FindMulti("scorpion", LAYER1, i); target) {

					// 피격 대상과 피격 대상에게 입힐 대미지를 입력한 후, true리턴 시 해당 대상은 입력한 대미지를 입게 된다.
					if(!target->GetDeathState() && target->CheckHit(XMFLOAT2(0.0, 0.0), 25))
						break;
				}
			}

			current_fire_delay = dest_fire_delay;
			crosshair_ptr->InputRecoil(0.1);

			gun_offset -= 0.1;
			int randnum = dist(gen);
			if (randnum == 1)
				dest_recoil_shake = 30.0;
			else
				dest_recoil_shake = -30.0;

			rotation.x -= 2.5;

			flame_time = 0.03;
		}
	}
}

void Player::UpdateGun(float FrameTime) {
	gun_offset = std::lerp(gun_offset, 0.0, FrameTime * 10.0);

	if (gun_zoomed && !gun_collided) {
		gun_position_offset.x = std::lerp(gun_position_offset.x, 0.0, FrameTime * 20.0);
		gun_position_offset.y = std::lerp(gun_position_offset.y, -0.22, FrameTime * 20.0);
		gun_position_offset.z = std::lerp(gun_position_offset.z, 0.4, FrameTime * 20.0);
	}

	else {
		gun_position_offset.x = std::lerp(gun_position_offset.x, 0.3, FrameTime * 20.0);
		gun_position_offset.y = std::lerp(gun_position_offset.y, -0.3, FrameTime * 20.0);
		gun_position_offset.z = std::lerp(gun_position_offset.z, 0.4, FrameTime * 20.0);
	}

	gun_rotation.x = std::lerp(gun_rotation.x, rotation.x, FrameTime * 30.0);
	gun_rotation.y = std::lerp(gun_rotation.y, rotation.y, FrameTime * 30.0);
	gun_rotation.z = std::lerp(gun_rotation.z, rotation.z, FrameTime * 30.0);
;
	if (gun_collided)
		gun_rotation_offset = std::lerp(gun_rotation_offset, -90.0, FrameTime * 3.0);
	else 
		gun_rotation_offset = std::lerp(gun_rotation_offset, 0.0, FrameTime * 3.0);
}

void Player::UpdateTerrainCollision(float FrameTime) {
	// 플레이어 높이가 항상 터레인 위에 위치하도록 한다
	if (auto terrain = scene.Find(target_terrain_name); terrain) {
		terr.InputPosition(position);
		terr.ClampToTerrain(terrain->GetTerrain(), position, 3.0);
	}
}

void Player::UpdateGunCollision() {
	for (auto const& O : map_oobb_data) {
		if (gun_oobb.CheckCollision(O)) {
			gun_collided = true;
			return;
		}
	}

	gun_collided = false;
}

void Player::UpdateCameraRotation() {
	// 상하 카메라 회전 제한
	Clamp::ClampValue(rotation.x, -90.0, 90.0, CLAMP_FIX);
	Clamp::ClampValue(rotation.z, -90.0, 90.0, CLAMP_FIX);

	// 벡터 및 카메라 추적 업데이트
	Math::UpdateVector(vec, rotation);
	camera.Track(position, vec, 0);
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
}

void Player::UpdateShootMotion(float FrameTime) {
	recoil_shake_num += FrameTime * 40.0;
	recoil_shake = std::lerp(recoil_shake, sinf(recoil_shake_num) * dest_recoil_shake, FrameTime * 5.0);

	if(trigger_state)
		dest_recoil_shake = std::lerp(dest_recoil_shake, 0.0, 5.0 * FrameTime);
	else
		dest_recoil_shake = std::lerp(dest_recoil_shake, 0.0, 20.0 * FrameTime);
}

void Player::UpdateCamera(float FrameTime) {
	// 카메라 회전 업데이트
	UpdateCameraRotation();


	// 카메라 워킹은 임시로 비활성화
	// 걷기 모션 업데이트
	//UpdateWalkMotion(FrameTime);

	// 총 반동 연출 업데이트
	//UpdateShootMotion(FrameTime);

	// 카메라 최종 회전
	camera.Rotate(rotation.x, rotation.y, rotation.z + walk_shake_result + recoil_shake);

	// 정조준 시 fov 업데이트
	global_fov_offset = std::lerp(global_fov_offset, fov_dest, FrameTime * 20.0);
}
