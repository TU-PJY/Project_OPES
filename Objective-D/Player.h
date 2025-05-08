#pragma once
#include "GameObject.h"
#include "Scene.h"
#include "ScriptUtil.h"
#include <random>

class Player : public GameObject {
private:
	// 위치
	XMFLOAT3 position{ -130.0, 20.0, -130.0 };

	// 각도 (바라보는 각도)
	XMFLOAT3 rotation{};

	// 플레이어가 받아올 대상 터레인의 이름
	std::string target_terrain_name{};

	// 터레인 충돌 처리를 위한 터레인 유틸 객체
	TerrainUtil terr{};

	// 카메라 추적을 위한 벡터 구조체
	Vector vec{};


	// 플레이어가 방아쇠를 당기고 있는 상태
	bool trigger_state{};

	// 현재 총 발사 딜레이 시간
	float current_fire_delay{};

	// 플레이어 초과된 발사 딜레이 시간
	float over_fire_delay{};

	// 플레이어 총 발사 간격
	float dest_fire_delay{ 0.1 };


	// 이동 상태 앛, 뒤, 좌, 우
	bool move_front{}, move_back{}, move_right{}, move_left{};

	// 앞뒤 속도 및 좌우 속도
	float forward_speed{}, strafe_speed{};

	// 플레이어 이동 속도
	float dest_move_speed{ 15.0 };

	// 걷기 모션 흔들림 업데이트 값
	float walk_shake_num{};

	// 걷기 모션 흔들림 수치 값
	float walk_shake_value{};

	// 걷기 모션 흔들림 결과 값
	float walk_shake_result{};


	// 1인칭 총 오프셋 위치
	float gun_offset{};

	// 1인칭 총 회전 각 오프셋
	float gun_rotation_offset{};

	// 총이 맵에 닿는 상태
	bool gun_collided{};

	// 정조준 상태
	bool gun_zoomed{};

	// 총 회전 각
	XMFLOAT3 gun_rotation{};

	// 총 위치
	XMFLOAT3 gun_position_offset{};

	// 총 바운드 박스
	// gun_collided 판정을 위함
	OOBB gun_oobb{};

	// 총 발사 시 흔들림 값
	float recoil_shake{};

	// 흔들림 업데이트 값
	float recoil_shake_num{};

	// 총 발사 시 목표 흔들림 값
	float dest_recoil_shake{};

	// 반동으로 인한 카메라 올라감
	float recoil_rotation{};

	// 정조준 시 카메라 fov 목표 값
	float fov_dest{};

	// 맵 벽 oobb 데이터
	std::vector<OOBB> map_oobb_data{};

	// 플레이어 충돌 범위
	BoundSphere player_sphere{};


	// 크로스헤어 오브젝트 포인터
	GameObject* crosshair_ptr{};

public:
	Player(std::string MapObjectName);
	void InputMouseMotion(MotionEvent& Event) override;
	void InputMouse(MouseEvent& Event);
	void InputKey(KeyEvent& Event);
	void UpdateMoveSpeed(float FrameTime);
	void UpdateFire(float FrameTime);
	void UpdateGun(float FrameTime);
	void UpdateCameraRotation();
	void UpdateTerrainCollision(float FrameTime);
	void UpdateGunCollision();
	void UpdateWalkMotion(float FrameTime);
	void UpdateShootMotion(float FrameTime);
	void Update(float FrameTime) override;
	void Render();
	void UpdateCamera(float FrameTime);
};