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

	// 체력
	int totalHP{ 200 };
	int currentHP{ 200 };

	// 현재 터레인 객체 이름/포인터 및 터레인 유틸
	std::string currentMapName{};
	GameObject* currentTerrain{};
	TerrainUtil terrainUtil{};

	// 맵 바운드 박스 모음, 맵 바운드 충돌처리를 위한 바운드
	std::vector<OOBB> mapBounds{};
	BoundSphere       playerSphere{};

	// 몬스터 - 플레이어 충돌처리용
	OOBB              playerBound{};

	// 현재 가지는 총기 객체 포인터
	GameObject*       weaponPtr{};

	// 플레이어 체력 인디케이터
	GameObject*       IndicatorPtr{};

public:
	Player1st(const std::string& terrainName, int characterType);
	~Player1st();
	void InputMouseMotion(MotionEvent& Event) override;
	void InputMouse(MouseEvent& Event) override;
	void InputKey(KeyEvent& Event) override;

	void updateState();
	void updateMove(float Delta);
	void updateTerrainCollision();
	void updateCamera(float Delta);
	void updateGun();
	void updateBound();

	void Update(float Delta) override;
	void Render() override;
	XMFLOAT3 GetPosition() override;
	OOBB GetOOBB() override;
	XMFLOAT3 GetSize() override;
	void InputRecoil(float Value) override;
	void GiveDamage(int damage) override;
};