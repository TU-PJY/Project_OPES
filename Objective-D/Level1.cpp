#include "ModePack.h"

#include "Map1.h"
#include "SkyBox.h"
#include "CameraController.h"
#include "CenterBuilding.h"
#include "RoadBlock.h"
#include "Player.h"
#include "CrossHair.h"
#include "Map1DefenseIndicator.h"
#include "MuzzleFlash.h"
#include "PlantMonster.h"
#include "MonsterGenerator.h"

namespace Level1 { 
	std::deque<GameObject*> ControlObjectList; 
}

bool dev = false;

void Level1::Start() {
	globalFovOffset = 0.0;
	// 맵1 디펜스 모드에 등장하는 몬스터 수를 20마리로 설정
	GLOBAL.map1DefenseEnemyRemained = 20;
	GLOBAL.map1DefenseState = true;

	scene.SetupMode("Level1", Destructor, ControlObjectList);

	scene.AddObject(new SkyBox, "skybox", LAYER1);
	scene.AddObject(new Map1, "map1", LAYER1);
	scene.AddObject(new CenterBuilding("map1", -2.0), "center_building", LAYER1);
	scene.AddObject(new RoadBlock(XMFLOAT3(-91.0, 5.0, -93.0), 20.0, 10), "roadBlock", LAYER1);

	// map1 몬스터를 20번 스폰하는 디펜스 모드 몬스터 제너레이터
	scene.AddObject(new DefenseModeMonsterGenerator("map1", GLOBAL.map1DefenseEnemyRemained), "defenseModeMonsterGenerator", LAYER1);

	if (dev)
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	else {
		scene.AddObject(new CrossHair, "crosshair", LAYERUI);
		scene.AddObject(new Player("map1"), "player", LAYER_PLAYER, true);
	}

	scene.AddObject(new Map1DefenseIndicator, "map1DefendeIndicator", LAYERUI);
}

void Level1::Destructor() {
}

