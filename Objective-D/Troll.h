#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Troll : public GameObject {
private:
	FBX trollFBX{ MESH.troll };

	enum TrollState {
		TROLL_IDLE,
		TROLL_MOVE,
		TROLL_ATTACK,
		TROLL_DEATH
	};

	int currentState{ TROLL_IDLE };
	int prevState{ -1 };
	int serverState{ -1 };

	int totalHP{ TROLL_HP };
	int currentHP{ TROLL_HP };

	xmfloat3 position{};
	xmfloat3 positionDest{};
	xmfloat3 rotation{};
	xmfloat3 rotationDest{};
	xmfloat3 size{ 2.0f, 2.0f, 2.0f };

	BoundSphere frustumBound{};
	BoundSphere attackBound{};
	BoundSphere trollBound{};
	BoundSphere lookRange{};
	OOBB trollOOBB{};
	bool inFrustum{};

	TerrainUtil terrainUtil{};

	GameObject* hpInd{};

	bool attackDid{};

	unsigned int ID{};

	float sendState{};
	float sendDelay{};
	float destDelay{ 1.0 / 30.0 };

	unsigned int currentTargetID{};
	unsigned int prevTargetID{};

public:
	Troll(const xmfloat3& createPosition, unsigned int ID);
	~Troll();
	void updateIndicator();
	void updateTerrainCollision();
	void updateState();
	void updateAnimation(float Delta);
	void updateBound();
	void updateMove(float Delta);
	void detectPlayer(float Delta);
	void updateAttack();
	void updateDeath();
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

