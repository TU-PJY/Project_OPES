#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Imp : public GameObject {
private:
	enum ImpStateEnum {
		IMP_IDLE,
		IMP_WALK,
		IMP_ATTACK,
		IMP_DEATH
	};

	FBX impFBX{ MESH.imp };

	int currentState{ IMP_IDLE };
	int prevState{ -1 };

	int totalHP{ GAZER_HP };
	int currentHP{ GAZER_HP };

	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 size{ 3.0f, 3.0f, 3.0f };

	BoundSphere frustumBound{};
	OOBB        attackBound{};
	OOBB        hitBox{};

	TerrainUtil terrainUtil{};

	bool     inFrustum{};
	bool     fallDown{};

	bool    attackDid{};

	unsigned int currentTargetID{};

public:
	Imp(const xmfloat3& createPosition, unsigned int ID);
	void im_updateTerrainCollision();
	void im_updateBound();
	void im_updateAnimation(float Delta);
	void im_updateState();
	void im_updateAttack();
	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
	bool CheckHit(BoundSphere& sphere) override;
	xmfloat3 GetPosition() override;
	void GiveDamage(int damage) override;
	void InputHP(int hp) override;
	bool GetDeathState() override;
};

