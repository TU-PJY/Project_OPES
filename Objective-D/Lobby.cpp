#include "Lobby.h"
#include "MouseUtil.h"

Lobby::Lobby() {
	
}

void Lobby::InputKey(KeyEvent& Event) {

}

void Lobby::InputMouse(MouseEvent& Event) {
	if (Event.Type == WM_LBUTTONDOWN) {
		for (int i = 0; i < 3; i++) {
			if (button[i].CheckCollisionPoint(xmfloat2(mouse.x, mouse.y))) {
				selectedCharacter = i;
				return;
			}
		}
	}
}

void Lobby::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 2.0 * ASPECT, 2.0);
	SetColor(0.2, 0.2, 0.2);
	Render2D(TEX.ColorTex);

	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 4.0, 4.0);
	Render2D(TEX.UI_lobbyBackground, 0.3);

	text.SetAlign(ALIGN_MIDDLE);
	text.Render(xmfloat2(0.0, 1.0 - 0.15), 0.3, "Lobby");
	text.SetAlign(ALIGN_LEFT);
	text.Render(xmfloat2(1.0 * ASPECT - 0.1, -1.0 + 0.1), 0.1, "Waiting for players to prepare...");

	text.SetAlign(ALIGN_DEFAULT);
	text.Render(xmfloat2(-1.0 * ASPECT + 0.2, 1.0 - 0.3), 0.2, "Players");

	float renderHeight = 1.0 - 0.4;
	for (auto& p : GLOBAL.playerList) {
		switch (p.second.characterType) {
		case -1: 
			{
				std::string renderStr = "ID: " + std::to_string(p.first) + " Name: " + p.second.name + " Type: Not Selected";
				break;
			}

		case 0:
			{
				std::string renderStr = "ID: " + std::to_string(p.first) + " Name: " + p.second.name + " Type: Heavy";
				break;
			}

		case 1:
			{
				std::string renderStr = "ID: " + std::to_string(p.first) + " Name: " + p.second.name + " Type: MarksMan";
				break;
			}

		case 2:
			{
				std::string renderStr = "ID: " + std::to_string(p.first) + " Name: " + p.second.name + " Type: Engineer";
				break;
			}
		}

		text.Render(xmfloat2(-1.0 * ASPECT + 0.2, renderHeight), 0.1, "Players");

		renderHeight -= 0.12;
	}

	text.Render(xmfloat2(-1.0 * ASPECT + 0.2, -0.25), 0.2, "Select Character");


	text.SetAlign(ALIGN_MIDDLE);

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3, -1.0 + 0.3);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.5);
	Render2D(TEX.UI_heavyIcon);
	if (selectedCharacter == 0)
		Render2D(TEX.UI_selected);

	text.Render(xmfloat2(-1.0 * ASPECT + 0.3, -1.0 + 0.6), 0.1, "HEAVY");

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3 + 0.55, -1.0 + 0.3);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.5);
	Render2D(TEX.UI_marksmanIcon);
	if (selectedCharacter == 1)
		Render2D(TEX.UI_selected);

	text.Render(xmfloat2(-1.0 * ASPECT + 0.3 + 0.55, -1.0 + 0.6), 0.1, "MARKS MAN");

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3 + 1.1, -1.0 + 0.3);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.5);
	Render2D(TEX.UI_engineerIcon);
	if (selectedCharacter == 2)
		Render2D(TEX.UI_selected);

	text.Render(xmfloat2(-1.0 * ASPECT + 0.3 + 1.1, -1.0 + 0.6), 0.1, "ENGINEER");
}

void Lobby::Update(float Delta) {
	button[0].Update(xmfloat3(-1.0 * ASPECT + 0.3, -1.0 + 0.3, 0.0), xmfloat3(0.25, 0.25, 0.0));
	button[1].Update(xmfloat3(-1.0 * ASPECT + 0.3 + 0.55, -1.0 + 0.3, 0.0), xmfloat3(0.25, 0.25, 0.0));
	button[2].Update(xmfloat3(-1.0 * ASPECT + 0.3 + 1.1, -1.0 + 0.3, 0.0), xmfloat3(0.25, 0.25, 0.0));
}
