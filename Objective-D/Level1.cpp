#include "ModePack.h"

#include "Map1.h"
#include "SkyBox.h"
#include "CameraController.h"
#include "CenterBuilding.h"
#include "Player.h"
#include "CrossHair.h"
#include "MuzzleFlash.h"
#include "PlantMonster.h"
#include "MonsterGenerator.h"

namespace Level1 { std::deque<GameObject*> ControlObjectList; }

bool dev = false;

void Level1::Start() {
	globalFovOffset = 0.0;
	scene.SetupMode("Level1", Destructor, ControlObjectList);

	scene.AddObject(new Map1, "map1", LAYER1);
	scene.AddObject(new SkyBox, "skybox", LAYER1);
	scene.AddObject(new CenterBuilding("map1", -2.0), "center_building", LAYER1);
//	scene.AddObject(new PlantMonster(XMFLOAT3(-140.0, 0.0, -120.0), "map1", true), "plantMonster", LAYER1);

	// map1 몬스터를 20번 스폰하는 디펜스 모드 몬스터 제너레이터
	scene.AddObject(new DefenseModeMonsterGenerator("map1", 20), "defenseModeMonsterGenerator", LAYER1);

	if (dev)
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	else {
		scene.AddObject(new CrossHair, "crosshair", LAYER3);
		scene.AddObject(new Player("map1"), "player", LAYER_PLAYER, true);
	}
}

void Level1::Destructor() {
}

