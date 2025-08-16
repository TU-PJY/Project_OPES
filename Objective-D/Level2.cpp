#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "CrossHair.h"
#include "SkyBox.h"
#include "CenterBuilding.h"
#include "MonsterSpawner.h"
#include "EditHelper.h"
#include "Player1st.h"
#include "OtherPlayer.h"
#include "OtherPlayerIndicator.h"
#include "Map1DefenseIndicator.h"
#include "MonsterGenerator.h"
#include "CrossHair.h"
#include "RoadBlock.h"

namespace Level2 { std::deque<GameObject*> ControlObjectList; }

void SendFilePacket(int stage, bool startDefense);

void Level2::Start() {
	scene.SetupMode("Level2", Destructor, ControlObjectList);
	GLOBAL.mapName = "map2";
	GLOBAL.mapOOBBdata.clear();
	GLOBAL.defenseIDList.clear();
	GLOBAL.Map2DefenseEnemyRemained = DEFENSE_MONSTER2;
	GLOBAL.Map2DefenseState = true;
	GLOBAL.offsetFOV = 0.0;
	GLOBAL.deathCount = 0;
	GLOBAL.stage = 2;

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
		scene.AddObject(new EditHelper, "editHelper", LAYER_UI2);
	}
	else
		scene.AddObject(new Player1st(GLOBAL.myCharacter), "player", LAYER_PLAYER, true);

	if (!GLOBAL.skipDefenseMode) {
		auto block = scene.AddObject(new RoadBlock(XMFLOAT3(-45.0, 1.0, -116.0), 90.0, 10), "roadBlock", LAYER1);
		GLOBAL.mapOOBBdata.emplace_back(block->GetOOBB());

		// map1 몬스터를 20번 스폰하는 디펜스 모드 몬스터 제너레이터
		scene.AddObject(new DefenseModeMonsterGenerator, "defenseModeMonsterGenerator", LAYER1);
	}


	if (!GLOBAL.skipTitleMode) {
		GLOBAL.otherIndicator = scene.AddObject(new OtherPlayerIndicator, "otherIndicator", LAYER_UI2);

		for (auto& p : GLOBAL.playerList) {
			std::cout << "currentCharacterType: " << p.second.characterType << std::endl;
			scene.AddObject(new OtherPlayer(p.second.characterType, p.first, p.second.name), std::to_string(p.first), LAYER_PLAYER);
			if (GLOBAL.otherIndicator)
				static_cast<GameObject*>(GLOBAL.otherIndicator)->AddPlayer(p.first, p.second.characterType, p.second.name);
		}
	}

	if (GLOBAL.skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpwaner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(GLOBAL.editMode), "monsterSpwaner", LAYER1, GLOBAL.editMode);

	if (!GLOBAL.skipDefenseMode)
		scene.AddObject(new DefenseIndicator, "map2DefenseIndicator", LAYER_UI2);

	scene.AddObject(new CrossHair, "crosshair", LAYER_UI2, true);

	SendFilePacket(2, true);
}

void Level2::Destructor() {

}