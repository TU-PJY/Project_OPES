#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class PlantMonster : public GameObject {
private:
	enum PlantMonsterState {
		PLANT_IDLE,
		PLANT_ATTACK,
		PLANT_DEATH,
		PLANT_LIFT
	};

	// 모델
	FBX         plantFBX{};

	// 위치, 회전 각도, 크기
	XMFLOAT3    position{};
	XMFLOAT3    tempPosition{};
	float       terrainFloorHeight{};
	XMFLOAT3    rotation{};
	XMFLOAT3    size{ XMFLOAT3(1.0, 1.0, 1.0) };

	// 각종 상태
	int         currentState{};
	int         prevState{};
	bool        behaviorEnabledState{};
	bool        detectState{};

	// 전체 체력, 현재 체력
	int         totalHP   = 100;
	int         currentHP = 100;

	// 땅 속에서 나온 경우만 사용. 스폰 후 행동 시작 지연 시간
	float       behaviorEnableDelayTime{};

	// 죽은 상태 활성화 이후 삭제 지연 시간
	float       deleteDelayTime{};

	// 플레이어 감지를 위한 시야
	BoundSphere lookRange{};

	// 충돌 감지를 위한 히트 박스
	OOBB        hitBox{};
	
	// 현재 객체가 소유하는 hp 표시기 오브젝트 포인터
	GameObject* hpIndicator{};

public:
	void updateHitBox();
	void updatePlayerDetect();
	void updateIndicatorHP();
	void updateAnimation(float Delta);
	void updateBehaviorEnableDelay(float Delta);
	void updateDeleteDelay(float Delta);

	PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, bool appearFromGround=false);
	void Update(float Delta) override;
	void Render()            override;
	OOBB GetOOBB()           override;
	void GiveDamage(int hp)  override;
	bool GetDeathState()     override;
};