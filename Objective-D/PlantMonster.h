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
	float       terrainHeight{};
	XMFLOAT3    rotation{};
	XMFLOAT3    size{ XMFLOAT3(3.0, 3.0, 3.0) };

	// 각종 상태
	int         currentState{};
	int         prevState{-1};
	bool        detectState{};
	bool        behaviorEnabledState{};

	// 디펜스 모드 실행 시 별도의 로직을 실행한다.
	bool        defenseModeState{};

	// 전체 체력, 현재 체력
	int         totalHP   = 100;
	int         currentHP = 100;

	// 죽은 상태 활성화 이후 삭제 지연 시간
	float       deleteDelayTime{};

	// 플레이어 감지를 위한 시야
	BoundSphere lookRange{};

	// 충돌 감지를 위한 히트 박스
	OOBB        hitBoxHead{};
	OOBB        hitBoxBody{};
	
	// 현재 객체가 소유하는 hp 표시기 오브젝트 포인터
	GameObject* hpIndicator{};

public:
	void updateTargetDetect();
	void updateIndicatorHP();
	void updateLiftFromGround(float Delta);
	void updateAnimation(float Delta);
	void updateDeleteDelay(float Delta);

	PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, bool appearFromGround=false);
	void Update(float Delta) override;
	void Render()            override;
	OOBB GetOOBB()           override;
	OOBB GetOOBB2()          override;
	void GiveDamage(int hp)  override;
	bool GetDeathState()     override;
};