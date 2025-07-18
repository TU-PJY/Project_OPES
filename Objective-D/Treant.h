#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class Treant : public GameObject {
private:
	enum treantState {
		TREANT_IDLE,
		TREANT_WALK,
		TREANT_ATTACK,
		TREANT_DEATH
	};

	FBX            treantFBX[4]{ MESH.treant[0], MESH.treant[1], MESH.treant[2], MESH.treant[3] };
	NodeDegreeData nodeDegree[4]{ {treantFBX[0], 5}, {treantFBX[1], 5}, {treantFBX[2], 5}, {treantFBX[3], 5} };

	XMFLOAT3       position{};
	XMFLOAT3       rotation{};
	XMFLOAT3       size{ XMFLOAT3(4.0, 4.0, 4.0) };
	int            currentState{ TREANT_WALK };
	int            prevState{ TREANT_IDLE };
	int            renderState{ TREANT_IDLE };

	std::string    currentMapName{};
	TerrainUtil    terrainUtil{};
	GameObject*    terrainPtr{};

	OOBB           oobb{};

	bool           inFrustum{};
	AABB           frustumAABB{};

public:
	Treant(const XMFLOAT3& createPosition, const std::string& terrainName);
	void updateTerrainCollision();
	void updateState();
	void updateAnimation(float Delta);
	void updateBound();
	void Update(float Delta) override;
	void Render() override;
};