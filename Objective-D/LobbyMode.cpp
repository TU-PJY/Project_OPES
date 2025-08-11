#include "ModePack.h"
#include "Lobby.h"

namespace LobbyMode { std::deque<GameObject*> ControlObjectList; }

void SendNamePacket(const char* playerName);

void LobbyMode::Start() {
	scene.SetupMode("LobbyMode", Destructor, ControlObjectList);

	//if(GLOBAL.useServer)
	if (!GLOBAL.enterServerState) {
		GLOBAL.netThread = std::thread(NetworkThread, GLOBAL.useLocalServer, GLOBAL.enterIPw.c_str());
		GLOBAL.enterServerState = true;
	}

	if (!GLOBAL.startedGameServer) {
		GLOBAL.serverConnected.wait(false);
		SendNamePacket(GLOBAL.myName.c_str());
		GLOBAL.startedGameServer = true;
	}

	scene.AddObject(new Lobby, "lobby", LAYERUI, true);
}

void LobbyMode::Destructor() {
}
