#include "Player.h"
#include "Scene.h"
#include "MouseUtil.h"
#include "CameraController.h"
#include "ClampUtil.h"

// �����ڿ��� �Է¹��� �� ������Ʈ �̸����� �ͷ��� ���� �޾ƿ´�.
Player::Player(std::string MapObjectName) {
	target_terrain_name = MapObjectName;
	mouse.StartMotionCapture(GlobalHWND);

	// ���� �ʿ��� �� oobb�� ���´�.
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
	// �� �߻� ������Ʈ
	UpdateFire(FrameTime);

	// �� - �� ������Ʈ �浹 ó�� ������Ʈ
	UpdateGunCollision();

	// �� ������Ʈ
	UpdateGun(FrameTime);

	// �̵� �ӵ� ������ ������Ʈ
	UpdateMoveSpeed(FrameTime);

	// ī�޶� ȸ�� ������Ʈ
	UpdateCameraRotation();

	// �ȱ� ��� ������Ʈ
	UpdateWalkMotion(FrameTime);

	// �ͷ��� �浹 ó�� ������Ʈ
	UpdateTerrainCollision(FrameTime);
}

void Player::Render() {
	// 1��Ī �� ������
	BeginRender();
	Transform::Move(TranslateMatrix, position.x, position.y, position.z);
	Transform::Rotate(TranslateMatrix, gun_rotation.x, gun_rotation.y + gun_rotation_offset, gun_rotation.z);
	Transform::Move(TranslateMatrix, 0.3, -0.3, 0.4 + gun_offset);
	Render3D(MESH.machine_gun, TEX.scifi, 1.0, DEPTH_TEST_FPS);

	Transform::Move(TranslateMatrix, 0.0, 0.0, 0.4);
	gun_oobb.Update(MESH.machine_gun, TranslateMatrix, RotateMatrix, ScaleMatrix, true);
	gun_oobb.Render();
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

	// ī�޶� ���� ȸ��
	camera.Rotate(rotation.x, rotation.y, rotation.z + walk_shake_result);
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

	// �÷��̾� �ٿ�� ����� ������Ʈ
	player_sphere.Update(position, 0.6);

	// OOBB�� �浹�� üũ�ϸ鼭 �̵�
	Math::MoveWithSlide(position, rotation.y, forward_speed, strafe_speed, player_sphere, MapOOBBData, FrameTime);
}

void Player::UpdateFire(float FrameTime) {
	// �� �߻� ������ ������Ʈ �Ѵ�.
	// dest_fire_delay �������� �߻��ϰ� �ȴ�.
	if (current_fire_delay > 0.0)
		current_fire_delay -= FrameTime;

	// �߻� ���¿��� current_fire_delay�� 0.0�� �Ǹ� crosshair�� �ݵ��� �ο� -> �߻�
	if (trigger_state) {
		if (current_fire_delay <= 0.0) {
			current_fire_delay = dest_fire_delay;
			if (auto crosshair = scene.Find("crosshair"); crosshair)
				crosshair->InputRecoil(0.1);

			gun_offset -= 0.1;
		}
	}
}

void Player::UpdateGun(float FrameTime) {
	gun_offset = std::lerp(gun_offset, 0.0, FrameTime * 10.0);

	gun_rotation.x = std::lerp(gun_rotation.x, rotation.x, FrameTime * 30.0);
	gun_rotation.y = std::lerp(gun_rotation.y, rotation.y, FrameTime * 30.0);
	gun_rotation.z = std::lerp(gun_rotation.z, rotation.z, FrameTime * 30.0);

	if (gun_collided)
		gun_rotation_offset = std::lerp(gun_rotation_offset, -90.0, FrameTime * 3.0);
	else
		gun_rotation_offset = std::lerp(gun_rotation_offset, 0.0, FrameTime * 3.0);
}

void Player::UpdateCameraRotation() {
	// ���� ī�޶� ȸ�� ����
	Clamp::ClampValue(rotation.x, -90.0, 90.0, CLAMP_FIX);
	Clamp::ClampValue(rotation.z, -90.0, 90.0, CLAMP_FIX);

	// ���� �� ī�޶� ���� ������Ʈ
	Math::UpdateVector(vec, rotation);
	camera.Track(position, vec, 0);
}

void Player::UpdateTerrainCollision(float FrameTime) {
	// �÷��̾� ���̰� �׻� �ͷ��� ���� ��ġ�ϵ��� �Ѵ�
	if (auto terrain = scene.Find(target_terrain_name); terrain) {
		terr.InputPosition(position);
		terr.ClampToTerrain(terrain->GetTerrain(), position, 3.0);
	}
}

void Player::UpdateGunCollision() {
	for (auto const& O : MapOOBBData) {
		if (gun_oobb.CheckCollision(O)) {
			gun_collided = true;
			return;
		}
	}

	gun_collided = false;
}