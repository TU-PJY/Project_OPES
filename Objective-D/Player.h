#pragma once
#include "GameObject.h"
#include "Scene.h"

class Player : public GameObject {
private:
	// 위치
	XMFLOAT3 position{ -130.0, 20.0, -130.0 };

	// 각도 (바라보는 각도)
	XMFLOAT3 rotation{};

	// 터레인 충돌 처리를 위한 터레인 유틸 객체
	TerrainUtil terr{};

	// 카메라 추적을 위한 벡터 구조체
	Vector vec{};

	// 이동 상태 앛, 뒤, 좌, 우
	bool move_front{}, move_back{}, move_right{}, move_left{};

	// 앞뒤 속도 및 좌우 속도
	float forward_speed{}, strafe_speed{};

	// 플레이어가 방아쇠를 당기고 있는 상태
	bool trigger_state{};

	// 현재 총 발사 딜레이 시간
	float current_fire_delay{};

	// 플레이어 초과된 발사 딜레이 시간
	float over_fire_delay{};

	// 플레이어 총 발사 간격
	float dest_fire_delay{ 0.1 };


	// 플레이어 이동 속도
	float dest_move_speed{ 15.0 };

	// 걷기 모션 흔들림 업데이트 값
	float walk_shake_num{};

	// 걷기 모션 흔들림 수치 값
	float walk_shake_value{};

	// 걷기 모션 흔들림 결과 값
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