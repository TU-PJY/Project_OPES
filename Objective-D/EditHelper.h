#pragma once
#include "Scene.h"
#include "TextUtil.h"
#include "TerrainUtil.h"

class EditHelper : public GameObject {
private:
	Text text{ ALIGN_MIDDLE, HEIGHT_DEFAULT, XMFLOAT3(1.0, 1.0, 1.0) };
	std::string renderStr{};
	XMFLOAT2    compassCenter{ XMFLOAT2(ASPECT * 1.0 - 0.6, -1.0 + 0.6) };
	XMFLOAT2    compassPos[4]{};
	TerrainUtil terrainUtil{};

	bool checkTerrain{};

public:
	EditHelper(bool terrainCheck=true);
	void Update(float Delta) override;
	void Render() override;
};