#include "ModePack.h"
#include "Lobby.h"

namespace LobbyMode { std::deque<GameObject*> ControlObjectList; }

void LobbyMode::Start() {
	scene.SetupMode("LobbyMode", Destructor, ControlObjectList);
	scene.AddObject(new Lobby, "lobby", LAYERUI, true);
}

void LobbyMode::Destructor() {
}
