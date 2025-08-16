#pragma once
#include "Scene.h"
#include "TextUtil.h"

class AdventureTimer : public GameObject {
private:
	Text text{ ALIGN_MIDDLE, HEIGHT_UNDER, XMFLOAT3(1.0, 1.0, 1.0) };
	float       renderHeight{ 1.5 };
	float       sizeOffset{};
	int         currentTime{};
	std::string renderString{};

public:
	AdventureTimer();
	void Update(float Delta) override;
	void Render() override;
	void InputTime(int time) override;
};