#include "ModePack.h"

#include "Map1.h"
#include "CameraController.h"
#include "CenterBuilding.h"
#include "Player.h"
#include "CrossHair.h"
#include "MuzzleFlash.h"
#include "PlantMonster.h"

namespace Level1 { std::deque<GameObject*> ControlObjectList; }

bool dev = false;

void Level1::Start() {
	globalFovOffset = 0.0;
	scene.SetupMode("Level1", Destructor, ControlObjectList);

	scene.AddObject(new Map1, "map1", LAYER1, true);
	scene.AddObject(new CenterBuilding("map1", -2.0), "center_building", LAYER1);
	scene.AddObject(new PlantMonster(XMFLOAT3(-140.0, 0.0, -120.0), "map1", true), "plantMonster", LAYER1);

	if (dev)
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	else {
		scene.AddObject(new CrossHair, "crosshair", LAYER3);
		scene.AddObject(new Player("map1"), "player", LAYER_PLAYER, true);
	}
}

void Level1::Destructor() {
}

