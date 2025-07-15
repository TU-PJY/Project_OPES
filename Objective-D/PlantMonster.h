#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class PlantMonster : public GameObject {
private:
	enum PlantMonsterState {
		PLANT_IDLE,
		PLANT_ATTACK,
		PLANT_DEATH
	};

	// 모델
	FBX         plantFBX{};

	// 위치, 회전 각도, 크기
	XMFLOAT3    position{};
	XMFLOAT3    rotation{};
	XMFLOAT3    size{ XMFLOAT3(1.0, 1.0, 1.0) };

	// 기본 상태는 IdleAttack으로 지정
	int         currentState = PLANT_IDLE;
	int         prevState    = PLANT_IDLE;
	bool        detectState{};

	int         totalHP{ 100 };
	int         currentHP{ 100 };

	// 죽은 상태 활성화 이후 삭제 지연 시간
	float       deleteDelayTime{};

	// 활성화 시 땅속에서 나옴, 생성자에서 변경 가능
	bool        fromGroundState{};

	// 플레이어 감지를 위한 시야
	BoundSphere lookRange{};

	// 충돌 감지를 위한 히트 박스
	OOBB         hitBox{};

	// 터레인 감지를 위한 터레인 유틸
	TerrainUtil terrainUtil{};

	// 현재 터레인 오브젝트 포인터
	GameObject* currentTerrain{};
	
	// 현재 객체가 소유하는 hp 표시기 오브젝트 포인터
	GameObject* hpIndicator{};

public:
	PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, bool appearFromGround=false);
	void updateLookRange();
	void updateHitBox();
	void updatePlayerDetect();
	void updateIndicatorHP();
	void updateAnimation(float Delta);
	void updateDeleteDelay(float Delta);

	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
	void GiveDamage(int hp) override;
	bool GetDeathState() override;
};