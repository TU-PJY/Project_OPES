// cpp code
#include "ModePack.h"

#include "Map3.h"
#include "CameraController.h"

namespace Level3 { std::deque<GameObject*> ControlObjectList; }

void Level3::Start() {
	scene.SetupMode("Level3", Destructor, ControlObjectList);

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new Map3, "map3", LAYER1, true);
}

void Level3::Destructor() {
}