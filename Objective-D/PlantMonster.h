#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class PlantMonster : public GameObject {
private:
	enum PlantMonsterState {
		PLANT_IDLE,
		PLANT_MOVE, // 상태 개수 맞추기 위함
		PLANT_ATTACK,
		PLANT_DEATH,
		PLANT_LIFT
	};

	// 모델
	FBX         plantFBX{};

	// 위치, 회전 각도, 크기
	XMFLOAT3    position{};
	XMFLOAT3    tempPosition{};
	XMFLOAT3    targetPosition{};
	float       terrainHeight{};
	XMFLOAT3    rotation{};
	XMFLOAT3    destRotation{};
	XMFLOAT3    size{ XMFLOAT3(3.0, 3.0, 3.0) };

	// 각종 상태
	int         currentState{ PLANT_IDLE };
	int         serverState{ PLANT_IDLE };
	int         prevState{ -1 };
	bool        detectState{};
	bool        behaviorEnabledState{};
	bool        shootState{};

	float       animationTime{};

	// 디펜스 모드 실행 시 별도의 로직을 실행한다.
	bool        defenseModeState{};

	// 전체 체력, 현재 체력
	int         totalHP   = PLANT_HP;
	int         currentHP = PLANT_HP;

	// 죽은 상태 활성화 이후 삭제 지연 시간
	float       deleteDelayTime{};

	// 플레이어 감지를 위한 시야
	BoundSphere lookRange{};

	// 충돌 감지를 위한 히트 박스
	OOBB        hitBox[3]{};
	
	// 프러스텀 컬링을 위한 바운드
	BoundSphere frustumBound{};
	bool        inFrustum{};
	
	// 현재 객체가 소유하는 hp 표시기 오브젝트 포인터
	GameObject* hpIndicator{};

	// 몬스터 아이디
	unsigned int ID{};

	unsigned int currentTargetID{};
	unsigned int prevTargetID{};

	float sendDelay{};
	float destDelay{1.0 / 24.0};
	bool sendState{};
	int sendOrder{};

public:
	void updateHitBox(float Delta);
	void sendCurrentState();
	void sendCurrentPosition();
	void updateTargetDetect(float Delta);
	void updateAttack(float Delta);
	void updateIndicatorHP();
	void updateLiftFromGround(float Delta);
	void updateAnimation(float Delta);
	void updateDeath(float Delta);

	PlantMonster(const XMFLOAT3& createPosition, unsigned int ID, bool appearFromGround =false);
	~PlantMonster();
	void Update(float Delta) override;
	void Render()            override;
	void GiveDamage(int Damage) override;
	XMFLOAT3 GetPosition() override;
	bool CheckHit(float& distance) override;
	bool CheckHit(BoundSphere& bound) override;
	bool CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) override;
	unsigned int GetID() override;
	void InputState(unsigned int state) override;
	void InputTargetID(unsigned int id) override;
	void InputRotation(float degrees) override;
	float GetAnimationTime() override;
	void SetAnimationTime(float Time) override;
	void InputHP(int currentHP) override;
	bool GetDeathState() override;
	bool GetBehaviorState() override;
};