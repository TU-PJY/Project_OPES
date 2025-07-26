#pragma once
#include "Scene.h"
#include "TextUtil.h"

class Title : public GameObject{
private:
	Text text{ ALIGN_MIDDLE, HEIGHT_MIDDLE, xmfloat3(1.0, 1.0, 1.0) };
	AABB button{};
	std::string ipStr{};

public:
	void InputMouse(MouseEvent& Event) override;
	void Render() override;
	void Update(float Delta) override;
};

