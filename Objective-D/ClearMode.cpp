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
	GLOBAL.imReady = false;

	GLOBAL.NetRunning = false;                 // ① 네트워크 루프 정지 플래그
	//if (GLOBAL.netThread.joinable())           // ③ 즉시 조인 가능
		//GLOBAL.netThread.join();
}

void ClearMode::Destructor() {
}
