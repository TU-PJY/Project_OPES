#include "ModePack.h"

#include "ClampUtil.h"
#include "CameraController.h"
#include "Map2.h"
#include "Player.h"
#include "CrossHair.h"


//class EnterEvent : public GameObject {
//public:
//	void Update(float FrameTime) override {
//		if (player_enter) {
//			scene.AddObject(new OtherPlayer, std::to_string(enter_player_id), LAYER1);
//			std::cout << "PLAYER ENTERED +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
//			player_enter = false;
//		}
//	}
//};


namespace ServerTestMode { std::deque<GameObject*> ControlObjectList; }
void ServerTestMode::Start() {
	//scene.SetupMode("ServerTestMode", Destructor, ControlObjectList);
	
//scene.AddObject(new EnterEvent, "enter_event", LAYER1);
	//scene.AddObject(new Map2, "map2", LAYER1);
	//scene.AddObject(new CrossHair, "crosshair", LAYER3);
	//scene.AddObject(new Player("map2"), "player", LAYER1, true);
}

void ServerTestMode::Destructor() {

}