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
	int         currentState{};
	int         prevState{-1};
	bool        detectState{};
	bool        behaviorEnabledState{};
	bool        shootState{};

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
	OOBB        hitBox[3]{};
	
	// 프러스텀 컬링을 위한 바운드
	BoundSphere frustumBound{};
	bool        inFrustum{};
	
	// 현재 객체가 소유하는 hp 표시기 오브젝트 포인터
	GameObject* hpIndicator{};

	std::string currentMapName{};

	// 맵이 가지는 바운드박스 데이터
	std::vector<OOBB> mapBoundData{};

public:
	void updateHitBox(float Delta);
	void updateTargetDetect();
	void updateAttack(float Delta);
	void updateIndicatorHP();
	void updateLiftFromGround(float Delta);
	void updateAnimation(float Delta);
	void updateDeleteDelay(float Delta);

	PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, bool appearFromGround=false);
	~PlantMonster();
	void Update(float Delta) override;
	void Render()            override;
	bool CheckHit(BoundSphere& Sphere, int damage) override;
};