#include "ModePack.h"

#include "Map1.h"
#include "CameraController.h"


namespace Level1 { std::deque<GameObject*> ControlObjectList; }

void Level1::Start() {
	scene.SetupMode("Level1", Destructor, ControlObjectList);

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new Map1, "map1", LAYER1, true);
}

void Level1::Destructor() {
}

