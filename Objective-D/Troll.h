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

	int totalHP{ 200 };
	int currentHP{ 200 };

	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 size{ 3.0f, 3.0f, 3.0f };

	BoundSphere frustumBound{};
	OOBB trollOOBB{};
	bool inFrustum{};

	TerrainUtil terrainUtil{};

	GameObject* hpInd{};

	unsigned int ID{};

public:
	Troll(const xmfloat3& createPosition, unsigned int ID);
	~Troll();
	void updateIndicator();
	void updateTerrainCollision();
	void updateState();
	void updateAnimation(float Delta);
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};

