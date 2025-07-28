#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Gazer : public GameObject {
private:
	enum GazerStateEnum {
		GAZER_IDLE,
		GAZER_WALK,
		GAZER_ATTACK,
		GAZER_DEATH
	};

	FBX gazerFBX{ MESH.gazer };

	int renderState{ GAZER_IDLE };
	int currentState{ GAZER_IDLE };
	int prevState{ -1 };

	int totalHP{ GAZER_HP };
	int currentHP{ GAZER_HP };

	xmfloat3 position{};
	xmfloat3 positionDest{};
	float heightOffset{};
	xmfloat3 rotation{};
	xmfloat3 rotationDest{};
	xmfloat3 size{ 3.0f, 3.0f, 3.0f };

	BoundSphere frustumBound{};
	BoundSphere gazerBound{};
	BoundSphere lookRange{};
	OOBB        hitBox{};
	OOBB        attackBound{};

	TerrainUtil terrainUtil{};

	bool     inFrustum{};

	bool attackDid{};

	unsigned int currentTargetID{};
	unsigned int prevTargetID{};
	unsigned int ID{};

	bool fallDown{};
	float fallAcc{};

	int serverState{};


	bool defenseState{};
	bool behaviorEnabled{};

	bool sendState{};
	float destDelay = { 1.0 / 30.0 };
	float sendDelay{};

	GameObject* hpInd{};

public:
	Gazer(const xmfloat3& createPosition, unsigned int ID, bool defenseState=false);
	~Gazer();
	void gz_updateIndicator();
	void gz_updateLift(float Delta);
	void gz_updateBound();
	void gz_updateAnimation(float Delta);
	void gz_updateAttack();
	void gz_updateTerrainCollision();
	void gz_updateState();
	void gz_detectPlayer(float Delta);
	void gz_updateMove(float Delta);
	void gz_updateDeath();
	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
	bool CheckHit(BoundSphere& sphere) override;
	bool CheckHit(float& distance) override;
	xmfloat3 GetPosition() override;
	void GiveDamage(int damage) override;
	void InputHP(int hp) override;
	bool GetDeathState() override;
	bool CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance);
	unsigned int GetID() override;
};

