#include "Title.h"
#include "MouseUtil.h"
#include "ModePack.h"

void Title::InputKey(KeyEvent& Event) {
	if (Event.Type == WM_CHAR) {
		if (!nameInputMode) {
			if ((Event.Key >= '0' && Event.Key <= '9') || Event.Key == '.') {
				GLOBAL.enterIP += Event.Key;
			}

			std::wstring copyWstr(GLOBAL.enterIP.begin(), GLOBAL.enterIP.end());
			GLOBAL.enterIPw = copyWstr;
		}

		else {
			GLOBAL.myName += Event.Key;
			std::cout << "input\n";
		}
	}

	else if (Event.Type == WM_KEYDOWN && Event.Key == VK_BACK) {
		if (!nameInputMode) {
			if (!GLOBAL.enterIP.empty()) {
				GLOBAL.enterIP.pop_back();
				if (GLOBAL.enterIP.back() == '.')
					GLOBAL.enterIP.pop_back();
			}
		}

		else {
			if (!GLOBAL.myName.empty())
				GLOBAL.myName.pop_back();
		}
	}
}

void Title::InputMouse(MouseEvent& Event) {
	if (Event.Type == WM_LBUTTONDOWN) {
		if (startButton.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y)))
			scene.SwitchMode(LobbyMode::Start);

		else if (ipButton.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y)))
			nameInputMode = false;

		else if (nameButton.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y)))
			nameInputMode = true;
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
	std::string renderStr = "-> " + GLOBAL.enterIP;
	text.Render(xmfloat2(0.0, 0.1), 0.1, renderStr);

	text.Render(xmfloat2(0.0, -0.1), 0.1, "Enter Player Name");
	renderStr = "-> " + GLOBAL.myName;
	text.Render(xmfloat2(0.0, -0.2), 0.1, renderStr);

	text.Render(xmfloat2(0.0, -0.4), 0.2, "Start Game");
}

void Title::Update(float Delta) {
	ipButton.Update(xmfloat3(0.0, 0.1, 0.0), xmfloat3(0.675, 0.1, 0.0));
	nameButton.Update(xmfloat3(0.0, -0.2, 0.0), xmfloat3(0.675, 0.1, 0.0));
	startButton.Update(xmfloat3(0.0, -0.4, 0.0), xmfloat3(0.675, 0.1, 0.0));
}
