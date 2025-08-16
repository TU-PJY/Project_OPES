#include "ModePack.h"
#include "Lobby.h"

namespace LobbyMode { std::deque<GameObject*> ControlObjectList; }

void SendNamePacket(const char* playerName);

void LobbyMode::Start() {
	scene.SetupMode("LobbyMode", Destructor, ControlObjectList);

	if (!GLOBAL.startedGameServer) {
		if (GLOBAL.netThread.joinable())
			GLOBAL.netThread.join();
	}
	
	if (!GLOBAL.NetRunning.load()) {
		GLOBAL.NetRunning.store(true);
		GLOBAL.netThread = std::thread(NetworkThread, GLOBAL.useLocalServer, GLOBAL.enterIPw.c_str());
	}

	if (!GLOBAL.startedGameServer) {
		GLOBAL.serverConnected.wait(false);
		SendNamePacket(GLOBAL.myName.c_str());
		GLOBAL.startedGameServer = true;
	}

	scene.AddObject(new Lobby, "lobby", LAYER_UI2, true);
}

void LobbyMode::Destructor() {
}
