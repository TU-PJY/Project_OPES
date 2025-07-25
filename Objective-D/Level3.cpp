// cpp code
#include "ModePack.h"
#include "Map3.h"
#include "CameraController.h"
#include "EditHelper.h"
#include "Player1st.h"
#include "SkyBox.h"
#include "CBVUtil.h"
#include "CenterBuilding.h"
#include "MonsterSpawner.h"

namespace Level3 { std::deque<GameObject*> ControlObjectList; }

void Level3::Start() {
	bool editMode = true;
	bool skipDefenseMode = true;

	scene.SetupMode("Level3", Destructor, ControlObjectList);

	GLOBAL.mapName = "map3";
	GLOBAL.offsetFOV = 0.0;
	GLOBAL.DefenseState = true;
	GLOBAL.DefenseEnemyRemained = 20;

	FOG_DATA FogData{
		{0.68, 0.28, 0.1}, // Fog Color
		0.0,   //   padding1

		500.0, // Fog Start
		{0.0, 0.0, 0.0}, // padding2

		900.0, // FogEnd
		{0.0, 0.0, 0.0} // padding3
	};
	CBVUtil::Reset(GlobalSystem.CmdList, FogCBV);
	CBVUtil::Create(GlobalSystem.Device, &FogData, sizeof(FOG_DATA), FogCBV);

	scene.AddObject(new SkyBox, "skybox", LAYER1);
	auto mapObject = scene.AddObject(new Map3, GLOBAL.mapName, LAYER1);
	auto centerObject = scene.AddObject(new CenterBuilding(7.0), "center_building", LAYER1);
	GLOBAL.mapTerrain = mapObject->GetTerrain();
	GLOBAL.mapOOBBdata = mapObject->GetMapWallOOBB();
	GLOBAL.mapOOBBdata.emplace_back(centerObject->GetOOBB());

	if (editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}
	else
		scene.AddObject(new Player1st(CHARACTER_MG), "player", LAYER_PLAYER, true);

	if (skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpawner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(editMode), "monsterSpawner", LAYER1, editMode);

}

void Level3::Destructor() {
}