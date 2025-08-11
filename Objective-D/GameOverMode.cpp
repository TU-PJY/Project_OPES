#include "ModePack.h"
#include "GameOverScreen.h"


namespace GameOverMode { std::deque<GameObject*> ControlObjectList; }

void GameOverMode::Start() {
	scene.SetupMode("GameOverMode", Destructor, ControlObjectList);
	// add works here
	scene.AddObject(new GameOverScreen, "goverscreen", LAYERUI);

	GLOBAL.playerList.clear();
}

void GameOverMode::Destructor() {
}