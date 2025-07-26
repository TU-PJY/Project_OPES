#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	enum spiderState {
		SCOR_IDLE,
		SCOR_WALK,
		SCOR_ATTACK,
		SCOR_DEATH,
		SCOR_LIFT // 상태 개수 맞추기 위함
	};

	FBX         scorpionFBX{ MESH.scorpion };
	XMFLOAT3    position{};
	XMFLOAT3    positionDest{};
	XMFLOAT3    rotation{};
	XMFLOAT3    rotationDest{};
	XMFLOAT3    size{ 3.0, 3.0, 3.0 };

	// 잔상 방지를 위해 지연 위치 입력을 실행한다.
	XMFLOAT3    inputedPosition{};
	// 해당 상태가 true일 때만 position이 inputedPosition을 반영한다.
	bool        positionInputedState{};

	int         currentState{ SCOR_IDLE };
	int         prevState{ -1 };
	int         serverState{ SCOR_IDLE };

	float       animationTime{};

	bool        attackDid{};

	int         totalHP{ 100 };
	int         currentHP{ 100 };
	GameObject* hpIndicator{};

	TerrainUtil terrainUtil{};

	AABB        frustumAABB{};
	bool        inFrustum{};

	OOBB        hitBox[3]{};

	BoundSphere lookRange{};
	BoundSphere scorBound{};
	BoundSphere attackBound{};

	// 몬스터 아이디
	unsigned int ID{};

	// 현재 추적 중인 아이디
	// 0이면 Idle이거나 나를 추적 중
	// 0이 아니면 다른 플레이어 추적 중
	unsigned int currentTargetID{};
	unsigned int prevTargetID{};

	// 서버로 전송하는 간격
	float sendDelay{};

	// 30프레인 간격으로 전송
	float destDelay{ 1.0 / 30.0 };

	// 전송 여부
	bool sendState{};

public:
	Scorpion(const XMFLOAT3& createPosition, unsigned int ID);
	~Scorpion();
	void updateInputedPosition();
	void updateBound(float Delta);
	void updateIndicator();
	void updateTerrain();
	void sendCurrentState();
	void sendCurrentPosition();
	void updateDetectPlayer(float Delta);
	void updateState();
	void updateAnimation(float Delta);
	void updateMove(float Delta);
	void updateDeath();
	void updateAttack();
	void Update(float Delta) override;
	void Render() override;
	XMFLOAT3 GetPosition() override;
	bool CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) override;
	bool CheckHit(float& distance) override;
	bool CheckHit(BoundSphere& bound) override;
	void GiveDamage(int damage) override;
	void InputHP(int currentHP) override;
	unsigned int GetID() override;
	void InputState(unsigned int state) override;
	void InputPosition(XMFLOAT3& position) override;
	void InputRotation(float degrees) override;
	void InputTargetID(unsigned int target) override;
	bool GetDeathState() override;
	int GetHP() override;
};

