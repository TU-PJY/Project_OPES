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
	int prevState{ TREANT_IDLE };

	// 렌더링용 상태 변수. 상태 변경 시 발생하는 잔상 방지를 위해 1프레임 늦게 업데이트 한다.
	int renderState{ TREANT_IDLE };

	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 size{ 4.0f, 4.0f, 4.0f };

	int totalHP{ 400 };
	int currentHP{ 400 };

	bool defenseMoveState{};

	TerrainUtil terrainUtil{};

	OOBB treantOOBB{};
	BoundSphere frustumBound{};
	bool inFrustum{};

	GameObject* hpInd{};

	unsigned int ID{};

public:
	Treant(const xmfloat3& createPosition, unsigned int ID, bool defenseModeState=false);
	~Treant();
	void updateIndicator();
	void updateState();
	void updateAnimation(float Delta);
	void updateTerrainCollision();
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};

