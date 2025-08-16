#include "Lobby.h"
#include "MouseUtil.h"
#include "ModePack.h"

void SendChooseJobPacket(unsigned int playerID, int job);
void SendReadyPacket(unsigned int id);

Lobby::Lobby() {
	
}

void Lobby::InputMouse(MouseEvent& Event) {
	if (Event.Type == WM_LBUTTONDOWN) {
		// 뒤로가기 버튼을 누르면 다시 타이틀로 되돌아 간다.
		if (!GLOBAL.imReady && backButton.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y))) {
			scene.SwitchMode(TitleMode::Start);
			return;
		}

		// 플레이어가 없을 경우 누를 수 없음
		// 서버를 사용하는 경우에 한 함
		if (GLOBAL.useServer) {
			if (GLOBAL.playerList.size() >= MIN_PLAYER_COUNT - 1) {
				// 한 번 준비되면 해제 불가
				// 캐릭터를 선택하지 않으면 준비 불가
				if (!GLOBAL.imReady && selectedCharacter != -1 && readyButton.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y))) {
					GLOBAL.imReady = !GLOBAL.imReady;
					GLOBAL.myCharacter = selectedCharacter;
					SendReadyPacket(GLOBAL.myID);
				}

				if (!GLOBAL.imReady) {
					for (int i = 0; i < 3; i++) {
						if (button[i].CheckCollisionPoint(xmfloat2(mouse.x, mouse.y))) {
							selectedCharacter = i;
							SendChooseJobPacket(GLOBAL.myID, selectedCharacter);
							return;
						}
					}
				}
			}
		}

		else {
			if (!GLOBAL.imReady && selectedCharacter != -1 && readyButton.CheckCollisionPoint(xmfloat2(mouse.x, mouse.y))) {
				GLOBAL.imReady = !GLOBAL.imReady;
				GLOBAL.myCharacter = selectedCharacter;
				//SendReadyPacket(GLOBAL.myID);
			}

			if (!GLOBAL.imReady) {
				for (int i = 0; i < 3; i++) {
					if (button[i].CheckCollisionPoint(xmfloat2(mouse.x, mouse.y))) {
						selectedCharacter = i;
						//SendChooseJobPacket(GLOBAL.myID, selectedCharacter);
						return;
					}
				}
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
	Transform::Scale2D(ScaleMatrix, 3.5, 3.5);
	Render2D(TEX.UI_lobbyBackground, 0.3);

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.125, 1.0 - 0.125);
	Transform::Scale2D(ScaleMatrix, 0.15, 0.15);
	Render2D(TEX.UI_back);

	text.SetAlign(ALIGN_MIDDLE);
	text.Render(xmfloat2(0.0, 1.0 - 0.15), 0.3, "Lobby");

	if (!GLOBAL.serverConnected)
		text.Render(xmfloat2(0.0, 0.0), 0.2, "Server not connected...");

	text.SetAlign(ALIGN_LEFT);
	text.Render(xmfloat2(1.0 * ASPECT - 0.05, -1.0 + 0.1), 0.08, "Waiting for players to prepare...");

	text.SetAlign(ALIGN_DEFAULT);
	text.Render(xmfloat2(-1.0 * ASPECT + 0.2, 1.0 - 0.3), 0.2, "Players");

	std::string renderStr{};
	float renderHeight = 1.0 - 0.45;

	for (auto& p : GLOBAL.playerList) {
		switch (p.second.characterType) {
		case -1: 
			{
				renderStr = "ID: " + std::to_string(p.first) + " | Name: " + p.second.name + " | Not Selected";
				break;
			}

		case 0:
			{
				renderStr = "ID: " + std::to_string(p.first) + " | Name: " + p.second.name + " | HEAVY";
				break;
			}

		case 1:
			{
				renderStr = "ID: " + std::to_string(p.first) + " | Name: " + p.second.name + " | MARKSMAN";
				break;
			}

		case 2:
			{
				renderStr = "ID: " + std::to_string(p.first) + " | Name: " + p.second.name + " | ENGINEER";
				break;
			}
		}

		if (!p.second.readyState)
			renderStr += " | NOT READY";
		else
			renderStr += " | READY";

		text.Render(xmfloat2(-1.0 * ASPECT + 0.2, renderHeight), 0.1, renderStr);

		renderHeight -= 0.12;
	}

	text.Render(xmfloat2(-1.0 * ASPECT + 0.2, -0.25), 0.2, "Select Character");


	text.SetAlign(ALIGN_MIDDLE);

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3, -1.0 + 0.3);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.5);
	Render2D(TEX.UI_heavyIcon);
	if (!GLOBAL.imReady && selectedCharacter == 0 || GLOBAL.imReady && GLOBAL.myCharacter == 0)
		Render2D(TEX.UI_selected);

	text.Render(xmfloat2(-1.0 * ASPECT + 0.3, -1.0 + 0.6), 0.1, "HEAVY");

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3 + 0.55, -1.0 + 0.3);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.5);
	Render2D(TEX.UI_marksmanIcon);
	if (!GLOBAL.imReady && selectedCharacter == 1 || GLOBAL.imReady && GLOBAL.myCharacter == 1)
		Render2D(TEX.UI_selected);

	text.Render(xmfloat2(-1.0 * ASPECT + 0.3 + 0.55, -1.0 + 0.6), 0.1, "MARKS MAN");

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3 + 1.1, -1.0 + 0.3);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.5);
	Render2D(TEX.UI_engineerIcon);
	if (!GLOBAL.imReady && selectedCharacter == 2 || GLOBAL.imReady && GLOBAL.myCharacter == 2)
		Render2D(TEX.UI_selected);

	text.Render(xmfloat2(-1.0 * ASPECT + 0.3 + 1.1, -1.0 + 0.6), 0.1, "ENGINEER");

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.3 + 1.65, -1.0 + 0.2);
	Transform::Scale2D(ScaleMatrix, 0.5, 0.25);
	SetColor(0.4, 0.4, 0.4);

	if (GLOBAL.imReady) {
		Render2D(TEX.ColorTex, 0.5);
		text.SetOpacity(0.5);
		text.Render(xmfloat2(-1.0 * ASPECT + 0.3 + 1.65, -1.0 + 0.2), 0.07, "READY");
	}
	else {
		Render2D(TEX.ColorTex);
		text.Render(xmfloat2(-1.0 * ASPECT + 0.3 + 1.65, -1.0 + 0.2), 0.07, "PRESS READY");
	}

	text.SetOpacity(1.0);
}

void Lobby::Update(float Delta) {
	button[0].Update(xmfloat3(-1.0 * ASPECT + 0.3, -1.0 + 0.3, 0.0), xmfloat3(0.25, 0.25, 0.0));
	button[1].Update(xmfloat3(-1.0 * ASPECT + 0.3 + 0.55, -1.0 + 0.3, 0.0), xmfloat3(0.25, 0.25, 0.0));
	button[2].Update(xmfloat3(-1.0 * ASPECT + 0.3 + 1.1, -1.0 + 0.3, 0.0), xmfloat3(0.25, 0.25, 0.0));
	backButton.Update(xmfloat3(-1.0 * ASPECT + 0.125, 1.0 - 0.125, 0.0), xmfloat3(0.075, 0.75, 0.0));
	readyButton.Update(xmfloat3(-1.0 * ASPECT + 0.3 + 1.65, -1.0 + 0.2, 0.0), xmfloat3(0.25, 0.125, 0.0));

	if (GLOBAL.imReady) {
		for (auto& p : GLOBAL.playerList) {
			StartGame = true;
			if (!p.second.readyState) {
				StartGame = false;
				break;
			}
		}

		if(StartGame)
			scene.SwitchMode(Level1::Start);
	}
}
