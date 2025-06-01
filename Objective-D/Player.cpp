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
// �����ڿ��� �Է¹��� �� ������Ʈ �̸����� �ͷ��� ���� �޾ƿ´�.
Player::Player(std::string MapObjectName) {
	target_terrain_name = MapObjectName;
	mouse.StartMotionCapture(GlobalHWND);

	// ���� �ʿ��� �� oobb�� ���´�.
	if (auto Map = scene.Find(MapObjectName); Map)
		map_oobb_data = Map->GetMapWallOOBB();

	if (auto building = scene.Find("center_building"); building)
		map_oobb_data.emplace_back(building->GetOOBB());

	// ������� ���Ҹ� ���� �̸� ũ�ν���� ������Ʈ �����͸� �����Ѵ�.
	if (auto Object = scene.Find("crosshair"))
		crosshair_ptr = Object;
}

void Player::InputMouseMotion(MotionEvent& Event) {
	if (GetCapture() == Event.CaptureState) {
		mouse.HideCursor();
		GetCapture();

		// ������ �� ������ �������� �����
		float sensivity = 0.08;
		if (gun_zoomed)  sensivity = 0.04;
		XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, sensivity);
		UpdateMotionRotation(rotation, Delta.x, Delta.y);
	}
}

void Player::InputMouse(MouseEvent& Event) {
	// �� �߻� ���� Ȱ��ȭ / ��Ȱ��ȭ
	switch (Event.Type) {
	case WM_LBUTTONDOWN:
		// ���콺 ��� ĸ�� ���°� ������ ���(������ ��ư ������ �ٸ� �����쿡 ��Ŀ�̵� ���)
		// ������ �����쿡 ��Ŭ������ ��Ŀ���ϸ� ��� ĸ�� ���°� �ٽ� Ȱ��ȭ �ȴ�.
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
	// �� ������ ������ ������ Ű
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'W', move_front);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'S', move_back);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'A', move_left);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'D', move_right);
}

// ���� ���ϸ� �����ϱ� ���� 0.05�� �������� ��Ŷ ���� 
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
	// �� �߻� ������Ʈ
	UpdateFire(FrameTime);

	// �� ������Ʈ
	UpdateGun(FrameTime);

	// �̵� �ӵ� ������ ������Ʈ
	UpdateMoveSpeed(FrameTime);
	 
	// ī�޶� ������Ʈ
	UpdateCamera(FrameTime);

	// �ͷ��� �浹 ó�� ������Ʈ
	UpdateTerrainCollision(FrameTime);

	// �� - �� ������Ʈ �浹 ó�� ������Ʈ
	UpdateGunCollision();

	// ������ ��Ŷ ����
	SendPacket(FrameTime);
}

void Player::Render() {
	// 1��Ī �� ������
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, gun_rotation.x, gun_rotation.y + gun_rotation_offset, gun_rotation.z);
	Transform::Move(TranslateMatrix, gun_position_offset.x, gun_position_offset.y, gun_position_offset.z + gun_offset);
	Render3D(MESH.machine_gun, TEX.scifi);

	// ���� ��Ʈ ������
	SetLightUse(DISABLE_LIGHT);
	Render3D(MESH.dot_machine_gun, TEX.scifi);

	// flame_time ���� �Ҳ� ������
	if (flame_time > 0.0) {
		// �Ҳ� ������
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
	}

	gun_oobb.Update(MESH.machine_gun, TranslateMatrix, RotateMatrix, ScaleMatrix, true);
	gun_oobb.Render();
}

void Player::UpdateMoveSpeed(float FrameTime) {
	// ������ Ȱ��ȭ �� �ش� �������� ����
	if (move_front && !move_back)
		forward_speed = std::lerp(forward_speed, dest_move_speed, 10.0 * FrameTime);
	if (move_back && !move_front)
		forward_speed = std::lerp(forward_speed, -dest_move_speed, 10.0 * FrameTime);
	if (move_right && !move_left)
		strafe_speed = std::lerp(strafe_speed, dest_move_speed, 10.0 * FrameTime);
	if (move_left && !move_right)
		strafe_speed = std::lerp(strafe_speed, -dest_move_speed, 10.0 * FrameTime);

	// ������ ��Ȱ��ȭ �Ǵ� ���� �ݴ� ���� �̵� Ȱ��ȭ �� ����
	if ((!move_front && !move_back) || (move_front && move_back)) 
		forward_speed = std::lerp(forward_speed, 0.0, 10.0 * FrameTime);
	
	if ((!move_right && !move_left) || (move_right && move_left)) 
		strafe_speed = std::lerp(strafe_speed, 0.0, 10.0 * FrameTime);

	// �÷��̾� �ٿ�� ����� ������Ʈ
	player_sphere.Update(position, 2.0);

	// OOBB�� �浹�� üũ�ϸ鼭 �̵�
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
	// �� �߻� ������ ������Ʈ �Ѵ�.
	// dest_fire_delay �������� �߻��ϰ� �ȴ�.
	if (current_fire_delay > 0.0)
		current_fire_delay -= FrameTime;

	if (flame_time > 0.0)
		flame_time -= FrameTime;

	// �߻� ���¿��� current_fire_delay�� 0.0�� �Ǹ� crosshair�� �ݵ��� �ο� -> �߻�
	if (trigger_state) {
		if (current_fire_delay <= 0.0) {

			// LAYER1�� �����ϴ� ��� ���͵鿡 ���� �ǰ� �˻�
			// ������ �ƴ� ���� ��� �� ������ �ǰ��� �߻��ϸ� �˻� ���� ����
			size_t size = scene.LayerSize(LAYER1);
			for (int i = 0; i < size; i++) {
				if (auto target = scene.FindMulti("scorpion", LAYER1, i); target) {

					// �ǰ� ���� �ǰ� ��󿡰� ���� ������� �Է��� ��, true���� �� �ش� ����� �Է��� ������� �԰� �ȴ�.
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
	// �÷��̾� ���̰� �׻� �ͷ��� ���� ��ġ�ϵ��� �Ѵ�
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
	// ���� ī�޶� ȸ�� ����
	Clamp::ClampValue(rotation.x, -90.0, 90.0, CLAMP_FIX);
	Clamp::ClampValue(rotation.z, -90.0, 90.0, CLAMP_FIX);

	// ���� �� ī�޶� ���� ������Ʈ
	Math::UpdateVector(vec, rotation);
	camera.Track(position, vec, 0);
}

void Player::UpdateWalkMotion(float FrameTime) {
	walk_shake_num += FrameTime * 10.0;

	// �ȱ� ���� Ȱ��ȭ �� ī�޶� ��鸮�� ������ �ش�.
	if (move_front || move_back || move_right || move_left)
		walk_shake_value = std::lerp(walk_shake_value, 1.5, FrameTime * 5.0);

	// �ȱ� ���°� ��Ȱ��ȭ�� ���¶�� ���� ��鸲�� ���δ�.
	else
		walk_shake_value = std::lerp(walk_shake_value, 0.0, FrameTime * 5.0);

	// ���� ��鸲 �� ���
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
	// ī�޶� ȸ�� ������Ʈ
	UpdateCameraRotation();


	// ī�޶� ��ŷ�� �ӽ÷� ��Ȱ��ȭ
	// �ȱ� ��� ������Ʈ
	//UpdateWalkMotion(FrameTime);

	// �� �ݵ� ���� ������Ʈ
	//UpdateShootMotion(FrameTime);

	// ī�޶� ���� ȸ��
	camera.Rotate(rotation.x, rotation.y, rotation.z + walk_shake_result + recoil_shake);

	// ������ �� fov ������Ʈ
	global_fov_offset = std::lerp(global_fov_offset, fov_dest, FrameTime * 20.0);
}
