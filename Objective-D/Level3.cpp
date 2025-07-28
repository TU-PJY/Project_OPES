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
#include "OtherPlayer.h"
#include "OtherPlayerIndicator.h"
#include "MonsterGenerator.h"
#include "RoadBlock.h"

void SendFilePacket(int stage);

namespace Level3 { std::deque<GameObject*> ControlObjectList; }

void Level3::Start() {
	scene.SetupMode("Level3", Destructor, ControlObjectList);
	GLOBAL.mapName = "map3";
	GLOBAL.mapOOBBdata.clear();
	GLOBAL.defenseIDList.clear();
	GLOBAL.Map3DefenseEnemyRemained = DEFENSE_MONSTER3;
	GLOBAL.Map3DefenseState = true;
	GLOBAL.offsetFOV = 0.0;
	GLOBAL.deathCount = 0;
	GLOBAL.stage = 3;

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

	if (GLOBAL.editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}
	else
		scene.AddObject(new Player1st(GLOBAL.myCharacter), "player", LAYER_PLAYER, true);

	if (!GLOBAL.skipTitleMode) {
		GLOBAL.otherIndicator = scene.AddObject(new OtherPlayerIndicator, "otherIndicator", LAYERUI);

		for (auto& p : GLOBAL.playerList) {
			std::cout << "currentCharacterType: " << p.second.characterType << std::endl;
			scene.AddObject(new OtherPlayer(p.second.characterType, p.first), std::to_string(p.first), LAYER_PLAYER);
			if (GLOBAL.otherIndicator)
				static_cast<GameObject*>(GLOBAL.otherIndicator)->AddPlayer(p.first, p.second.characterType, std::to_string(p.first));
		}
	}

	if (GLOBAL.skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpawner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(GLOBAL.editMode), "monsterSpawner", LAYER1, GLOBAL.editMode);

	if (!GLOBAL.skipDefenseMode) {
		auto block = scene.AddObject(new RoadBlock(XMFLOAT3(-142.0 ,0.0, -46.0), 150.0 + 180.0, 5), "roadBlock", LAYER1);
		GLOBAL.mapOOBBdata.emplace_back(block->GetOOBB());

		scene.AddObject(new DefenseModeMonsterGenerator, "defenseModeMonsterGenerator", LAYER1);
	}


	SendFilePacket(3);
}

void Level3::Destructor() {
}