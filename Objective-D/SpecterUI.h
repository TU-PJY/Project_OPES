#pragma once
#include "Scene.h"
#include "TextUtil.h"

class SpecterUI : public GameObject {
private:
	Text text{ ALIGN_MIDDLE, HEIGHT_DEFAULT, xmfloat3(1.0, 1.0, 1.0) };;

public:
	SpecterUI();
	void Render() override;
};


