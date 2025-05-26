#pragma once
#include "GameObject.h"
#include "CollisionUtil.h"
#include "TerrainUtil.h"

class CenterBuilding : public GameObject {
private:
	XMFLOAT3 position{ -120.0, 0.0, -120.0 };
	OOBB oobb{};
	TerrainUtil terrainUT{};

public:
	CenterBuilding(std::string map_name, float height_offset);
	void Render();
};