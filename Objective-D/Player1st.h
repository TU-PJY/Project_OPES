#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

// 기존의 플레이어 클래스 대체
class Player1st : public GameObject {
private:
	enum playerMove {
		FRONT,
		RIGHT,
		LEFT,
		BACK
	};

	int characterType{};

	// 4 방향 이동 상태
	bool     moveState[4]{};

	// position: 실제 위치, camPosition: 카메라 위치
	// 실제 위치와 카메라 위치의 높이는 3.0 차이난다.
	XMFLOAT3 playerPosition{ -140.0, 20.0, -130.0 };
	XMFLOAT3 cameraPosition{};

	// 현재 회전값, 이전 회전값, 카메라 추적을 위한 벡터
	XMFLOAT3 currentRotation{};
	XMFLOAT3 prevRotation{};
	Vector   playerVector{};

	// 크기
	XMFLOAT3 playerSize{ 2.0, 2.0, 2.0 };

	// 현재 상태 및 이전 상태. 이전 상태와 현재 상태가 다를때만 패킷을 전송한다.
	int      currentState{ STATE_IDLE };
	int      prevState{ STATE_IDLE };
	int      serverState{ STATE_IDLE };

	// 방아쇠를 당긴 상태. 활성화 되면 무기 객체가 동작한다.
	bool     triggerState{};

	// 정조준을 한 상태. 활성화 되면 FOV가 변경되고 무기 객체가 동작한다.
	bool     zoomState{};

	// 카메라 목표 FOV
	float    destFOV{};

	// 앞, 옆, 최대 이동 속도
	float    forwardSpeed{};
	float    strafeSpeed{};
	float    maxSpeed{};
	float    currentSpeed{};

	// 플레이어 이동 속도 가감속 속도
	float    speedAcc{};

	// 체력
	int totalHP{};
	int prevHP{};
	int currentHP{};

	// 맵3용 변수
	bool fallDown{};
	float fallAcc{};

	// 엔지니어 전용 변수
	float turretCoolTime{};
	float beaconCoolTime{};

	// 플레이어 당 2개 사용 가능
	int currentGrenadeCount{ 2 };

	TerrainUtil terrainUtil{};

	BoundSphere       playerSphere{};

	// 몬스터 - 플레이어 충돌처리용
	OOBB              playerBound{};

	// 현재 가지는 총기 객체 포인터
	GameObject* weaponPtr{};

	// 플레이어 체력 인디케이터
	GameObject* IndicatorPtr{};

	GameObject* installPtr{};

	GameObject* scopePtr{};


	// 서버용 변수
	// 30프레임 간격으로 패킷을 전송한다.
	float             currentPacketSendDelay{};
	float             packetSendDelay{ 1.0 / 60.0 };
	int               sendOrder{1};

	float knockbackRotation{};
	float knockbackPower{};

	float footstepTime{};
	float footstepInterval{ 0.4 };

public:
	Player1st(int characterType);
	~Player1st();
	void sendPacket(float Delta);
	void InputMouseMotion(MotionEvent& Event) override;
	void InputMouse(MouseEvent& Event) override;
	void InputKey(KeyEvent& Event) override;

	void updateState();
	void updateMove(float Delta);
	void updateTerrainCollision();
	void updateCamera(float Delta);
	void updateGun();
	void updateBound();

	void updateIndicator();

	void Update(float Delta) override;
	void Render() override;
	XMFLOAT3 GetPosition() override;
	OOBB GetOOBB() override;
	XMFLOAT3 GetSize() override;
	void InputRecoil(float Value) override;
	void GiveHeal(int healHP);
	void GiveDamage(int damage) override;
	void InputHP(int currentHP) override;
	unsigned int GetID() override;
	void GiveKnockback(float rotation, float power) override;
};