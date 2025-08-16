#include "ModePack.h"
#include "ClearScreen.h"

namespace ClearMode { std::deque<GameObject*> ControlObjectList; }

void ClearMode::Start() {
	scene.SetupMode("ClearMode", Destructor, ControlObjectList);
	// add works here

	scene.AddObject(new ClearScreen, "cscreen", LAYER_UI2);

	GLOBAL.playerList.clear();
	GLOBAL.serverConnected.store(false);
	GLOBAL.startedGameServer = false;
}

void ClearMode::Destructor() {
}
