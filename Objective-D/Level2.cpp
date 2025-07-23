#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "CrossHair.h"
#include "SkyBox.h"
#include "CenterBuilding.h"
#include "MonsterSpawner.h"
#include "EditHelper.h"

namespace Level2 { std::deque<GameObject*> ControlObjectList; }


void Level2::Start() {
	bool editMode = false;

	scene.SetupMode("Level2", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map2";
	GLOBAL.map2DefenseState = true;
	GLOBAL.map2DefenseEnemyRemained = 20;

	// 맵이 반드시 Player보다 먼저 추가되어야 한다
	// 플레이어 객체 생성자에서 맵 데이터를 받아야 하기 때문
	scene.AddObject(new SkyBox, "skybox", LAYER1);
	auto mapObject = scene.AddObject(new Map2, "map2", LAYER1, true);
	auto centerObject = scene.AddObject(new CenterBuilding(-2.0), "center_building", LAYER1);

	GLOBAL.mapTerrain = mapObject->GetTerrain();
	GLOBAL.mapOOBBdata = mapObject->GetMapWallOOBB();
	GLOBAL.mapOOBBdata.emplace_back(centerObject->GetOOBB());
	
	if (editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}

	//scene.AddObject(new MonsterSpawner(editMode), "monsterSpwaner", LAYER1, true);

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
	GLOBAL.mapOOBBdata.clear();
}