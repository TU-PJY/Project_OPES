#pragma once
#include "Scene.h"
#include "TextUtil.h"

class Map1DefenseIndicator : public GameObject {
private:
	Text text{ ALIGN_MIDDLE, HEIGHT_UNDER, XMFLOAT3(1.0, 1.0, 1.0) };
	float       renderHeight{};
	float       sizeOffset{};
	int         totalRemain{};
	int         currentRemain{};
	int         prevRemain{};
	std::string renderString{};

public:
	Map1DefenseIndicator();
	void Update(float Delta) override;
	void Render() override;
};