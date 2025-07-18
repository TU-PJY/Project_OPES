#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	enum spiderState {
		SCOR_IDLE,
		SCOR_WALK,
		SCOR_ATTACK,
		SCOR_DEATH
	};

	FBX         scorpionFBX{ MESH.scorpion };
	XMFLOAT3    position{};
	XMFLOAT3    rotation{};
	XMFLOAT3    size{ 2.0, 2.0, 2.0 };

	int         currentState{ SCOR_DEATH };
	int         prevState{ SCOR_IDLE };

	int         totalHP{ 80 };
	int         currentHP{80};
	GameObject* hpIndicator{};

	TerrainUtil terrainUtil{};
	std::string currentMapName{};
	GameObject* currentTerrain{};

	AABB        frustumAABB{};
	bool        inFrustum{};

	OOBB        hitBox[3]{};


public:
	Scorpion(const XMFLOAT3& createPosition, const std::string& terrainName);
	~Scorpion();
	void updateBound(float Delta);
	void updateIndicator();
	void updateTerrain();
	void updateState();
	void updateAnimation(float Delta);
	void Update(float Delta) override;
	void Render() override;
};

