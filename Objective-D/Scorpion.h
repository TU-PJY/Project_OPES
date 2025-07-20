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

	int         currentState{ SCOR_IDLE };
	int         prevState{ SCOR_IDLE };

	int         totalHP{ 80 };
	int         currentHP{80};
	GameObject* hpIndicator{};

	TerrainUtil terrainUtil{};
	std::string currentMapName{};
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
	Scorpion(const XMFLOAT3& createPosition, const std::string& terrainName, unsigned int ID);
	~Scorpion();
	void updateBound(float Delta);
	void updateIndicator();
	void updateTerrain();
	void updateDetectPlayer();
	void updateState();
	void updateAnimation(float Delta);
	void updateMove(float Delta);
	void Update(float Delta) override;
	void Render() override;
};

