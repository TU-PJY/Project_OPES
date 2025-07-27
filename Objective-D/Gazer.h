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

	int currentState{ GAZER_IDLE };
	int prevState{ -1 };

	int totalHP{ GAZER_HP };
	int currentHP{ GAZER_HP };

	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 size{ 3.0f, 3.0f, 3.0f };

	BoundSphere frustumBound{};
	OOBB        hitBox{};
	OOBB        attackBound{};

	TerrainUtil terrainUtil{};

	bool     inFrustum{};

	bool attackDid{};

	unsigned int currentTargetID{};

public:
	Gazer(const xmfloat3& createPosition, unsigned int ID);
	void gz_updateBound();
	void gz_updateAnimation(float Delta);
	void gz_updateAttack();
	void gz_updateTerrainCollision();
	void gz_updateState();
	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
	bool CheckHit(BoundSphere& sphere) override;
	xmfloat3 GetPosition() override;
	void GiveDamage(int damage) override;
	void InputHP(int hp) override;
	bool GetDeathState() override;
};

