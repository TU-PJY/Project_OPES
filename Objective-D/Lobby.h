#pragma once
#include "Scene.h"
#include "TextUtil.h"

class Lobby : public GameObject {
private:
	// 직업 선택 버튼
	AABB button[3]{};
	Text text{ ALIGN_MIDDLE, HEIGHT_MIDDLE, xmfloat3(1.0, 1.0, 1.0)};
	Text playerText{ ALIGN_DEFAULT, HEIGHT_MIDDLE, xmfloat3(1.0, 1.0, 1.0) };

	int selectedCharacter{-1};

public:
	Lobby();
	void InputKey(KeyEvent& Event) override;
	void InputMouse(MouseEvent& Event) override;
	void Render() override;
	void Update(float Delta) override;
};