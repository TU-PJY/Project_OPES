#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class InstallIndicator : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 rotation{};
	TerrainUtil terrainUtil{};

	int currentItem{};
	bool renderState{};

public:
	void SetItem(int num) override;
	void SetRenderState(bool Flag)override;
	void Render() override;

};

