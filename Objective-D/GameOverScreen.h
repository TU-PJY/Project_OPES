#pragma once
#include "Scene.h"
#include "TextUtil.h"

class GameOverScreen : public GameObject {
private:
	Text text{ ALIGN_MIDDLE, HEIGHT_MIDDLE, xmfloat3(1.0, 1.0, 1.0) };

public:
	void Render() override;
};

