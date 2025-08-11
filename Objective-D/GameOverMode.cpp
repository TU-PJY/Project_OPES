#include "ModePack.h"
#include "GameOverScreen.h"


namespace GameOverMode { std::deque<GameObject*> ControlObjectList; }

void GameOverMode::Start() {
	scene.SetupMode("GameOverMode", Destructor, ControlObjectList);
	// add works here
	scene.AddObject(new GameOverScreen, "goverscreen", LAYER_UI2);

	GLOBAL.playerList.clear();
	GLOBAL.serverConnected.store(false);
	GLOBAL.startedGameServer = false;
}

void GameOverMode::Destructor() {
}