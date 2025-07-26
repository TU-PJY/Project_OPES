#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Treant : public GameObject {
private:
	FBX treantFBX[4]{ MESH.treant[0], MESH.treant[1], MESH.treant[2], MESH.treant[3] };

	enum TreantState {
		TREANT_IDLE,
		TREANT_MOVE,
		TREANT_ATTACK,
		TREANT_DEATH
	};

	int currentState{ TREANT_IDLE };
	int prevState{ -1 };
	int serverState{ -1 };

	// 렌더링용 상태 변수. 상태 변경 시 발생하는 잔상 방지를 위해 1프레임 늦게 업데이트 한다.
	int renderState{ TREANT_IDLE };

	xmfloat3 position{};
	xmfloat3 positionDest{};
	xmfloat3 rotation{};
	xmfloat3 rotationDest{};
	xmfloat3 size{ 3.0f, 3.0f, 3.0f };

	int totalHP{ 400 };
	int currentHP{ 400 };

	bool defenseMoveState{};

	TerrainUtil terrainUtil{};

	OOBB treantOOBB{};
	BoundSphere frustumBound{};
	BoundSphere attackBound{};
	BoundSphere treantBound{};
	bool inFrustum{};

	BoundSphere lookRange{};

	GameObject* hpInd{};

	unsigned int ID{};

	float sendState{};
	float sendDelay{};
	float destDelay{ 1.0 / 30.0 };

	unsigned int currentTargetID{};
	unsigned int prevTargetID{};


public:
	Treant(const xmfloat3& createPosition, unsigned int ID, bool defenseModeState=false);
	~Treant();
	void updateIndicator();
	void updateState();
	void updateAnimation(float Delta);
	void updateMove(float Delta);
	void detectPlayer(float Delta);
	void updateTerrainCollision();
	void updateBound();
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

