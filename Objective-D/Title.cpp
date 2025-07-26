#include "Title.h"
#include "MouseUtil.h"
#include "ModePack.h"

void Title::InputMouse(MouseEvent& Event) {
	if (Event.Type == WM_LBUTTONDOWN) {
		if(button.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y)))
			scene.SwitchMode(LobbyMode::Start);
	}
}

void Title::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 2.0 * ASPECT, 2.0);
	SetColor(0.2, 0.2, 0.2);
	Render2D(TEX.ColorTex);

	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 3.5, 3.5);
	Render2D(TEX.UI_lobbyBackground, 0.3);

	text.Render(xmfloat2(0.0, 1.0 - 0.35), 0.8, "OPES");

	text.Render(xmfloat2(0.0, 0.2), 0.1, "Enter Server IP");
	std::string renderStr = "-> " + ipStr;
	text.Render(xmfloat2(0.0, 0.1), 0.1, renderStr);

	text.Render(xmfloat2(0.0, -0.15), 0.3, "Start Game");
}

void Title::Update(float Delta) {
	button.Update(xmfloat3(0.0, -0.15, 0.0), xmfloat3(0.675, 0.1, 0.0));
}
