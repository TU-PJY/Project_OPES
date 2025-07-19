#pragma once
#include "Scene.h"

class PlayerHit : public GameObject {
private:
	float opacity{ 1.0 };

public:
	void Update(float Delta) override;
	void Render() override;
};