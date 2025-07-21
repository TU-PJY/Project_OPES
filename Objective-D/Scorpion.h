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
	XMFLOAT3    rotation{};
	XMFLOAT3    rotationDest{};
	XMFLOAT3    size{ 3.0, 3.0, 3.0 };

	// 잔상 방지를 위해 지연 위치 입력을 실행한다.
	XMFLOAT3    inputedPosition{};
	// 해당 상태가 true일 때만 position이 inputedPosition을 반영한다.
	bool        positionInputedState{};

	int         currentState{ SCOR_IDLE };
	int         prevState{ SCOR_IDLE };

	int         totalHP{ 80 };
	int         currentHP{80};
	GameObject* hpIndicator{};

	TerrainUtil terrainUtil{};
	GameObject* currentTerrain{};
	std::vector<OOBB> mapBounds{};

	AABB        frustumAABB{};
	bool        inFrustum{};

	OOBB        hitBox[3]{};

	BoundSphere lookRange{};
	BoundSphere scorBound{};
	BoundSphere attackBound{};

	// 몬스터 아이디
	unsigned int ID{};


public:
	Scorpion(const XMFLOAT3& createPosition, unsigned int ID);
	~Scorpion();
	void updateBound(float Delta);
	void updateIndicator();
	void updateTerrain();
	void updateDetectPlayer();
	void updateState();
	void updateAnimation(float Delta);
	void updateMove(float Delta);
	void updateDeath();
	void Update(float Delta) override;
	void Render() override;
	bool CheckHit(float& distance) override;
	void GiveDamage(int damage) override;
	unsigned int GetID() override;
	void InputState(unsigned int state) override;
	void InputPosition(XMFLOAT3& position) override;
};

