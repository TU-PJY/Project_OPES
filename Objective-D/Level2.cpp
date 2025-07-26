#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "CrossHair.h"
#include "SkyBox.h"
#include "CenterBuilding.h"
#include "MonsterSpawner.h"
#include "EditHelper.h"
#include "Player1st.h"

namespace Level2 { std::deque<GameObject*> ControlObjectList; }


void Level2::Start() {
	scene.SetupMode("Level2", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map2";
	GLOBAL.DefenseState = true;
	GLOBAL.DefenseEnemyRemained = 20;

	FOG_DATA FogData{
		{0.63, 0.77, 0.98}, // Fog Color
		0.0,   //   padding1

		300.0, // Fog Start
		{0.0, 0.0, 0.0}, // padding2

		500.0, // FogEnd
		{0.0, 0.0, 0.0} // padding3
	};
	CBVUtil::Reset(GlobalSystem.CmdList, FogCBV);
	CBVUtil::Create(GlobalSystem.Device, &FogData, sizeof(FOG_DATA), FogCBV);

	// 맵이 반드시 Player보다 먼저 추가되어야 한다
	// 플레이어 객체 생성자에서 맵 데이터를 받아야 하기 때문
	scene.AddObject(new SkyBox, "skybox", LAYER1);
	auto mapObject = scene.AddObject(new Map2, "map2", LAYER1, GLOBAL.editMode);
	auto centerObject = scene.AddObject(new CenterBuilding(-2.0), "center_building", LAYER1);

	GLOBAL.mapTerrain = mapObject->GetTerrain();
	GLOBAL.mapOOBBdata = mapObject->GetMapWallOOBB();
	GLOBAL.mapOOBBdata.emplace_back(centerObject->GetOOBB());
	
	if (GLOBAL.editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}
	else
		scene.AddObject(new Player1st(CHARACTER_MG), "player", LAYER_PLAYER, true);

	if (GLOBAL.skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpwaner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(GLOBAL.editMode), "monsterSpwaner", LAYER1, GLOBAL.editMode);

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
	GLOBAL.mapOOBBdata.clear();
}