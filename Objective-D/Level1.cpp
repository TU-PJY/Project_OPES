#include "ModePack.h"

#include "Map1.h"
#include "CameraController.h"
#include "CenterBuilding.h"

namespace Level1 { std::deque<GameObject*> ControlObjectList; }

void Level1::Start() {
	global_fov_offset = 0.0;
	scene.SetupMode("Level1", Destructor, ControlObjectList);

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new Map1, "map1", LAYER1, true);
	scene.AddObject(new CenterBuilding("map1", -2.0), "center_building", LAYER1);
}

void Level1::Destructor() {
}

