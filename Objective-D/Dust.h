#pragma once
#include "Scene.h"

class Dust : public GameObject {
private:
	xmfloat3 position{};
	xmfloat3 size{};
	xmfloat3 sizeDest{};
	xmfloat3 color{};
	float currentPlayTime{};
	int   currentFrame{};
	float opacity{1.0};
	
public:
	Dust(const xmfloat3& createPosition, const xmfloat3& createSize, const xmfloat3& destSize, const xmfloat3& createColor);
	void Update(float Delta) override;
	void Render() override;
};

