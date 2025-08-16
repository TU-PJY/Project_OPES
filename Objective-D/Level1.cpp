#include "ModePack.h"

#include "Map1.h"
#include "SkyBox.h"
#include "CameraController.h"
#include "CenterBuilding.h"
#include "RoadBlock.h"
#include "Player1st.h"
#include "CrossHair.h"
#include "Map1DefenseIndicator.h"
#include "MuzzleFlash.h"
#include "PlantMonster.h"
#include "MonsterGenerator.h"
#include "MonsterSpawner.h"
#include "EditHelper.h"
#include "OtherPlayer.h"
#include "OtherPlayerIndicator.h"
#include "FallingStone.h"

void SendFilePacket(int stage);

namespace Level1 { 
	std::deque<GameObject*> ControlObjectList; 
}

void Level1::Start() {
	scene.SetupMode("Level1", Destructor, ControlObjectList);
	GLOBAL.mapName = "map1";
	GLOBAL.mapOOBBdata.clear();
	GLOBAL.defenseIDList.clear();
	GLOBAL.Map1DefenseEnemyRemained = DEFENSE_MONSTER1;
	GLOBAL.Map1DefenseState = true;
	GLOBAL.offsetFOV = 0.0;
	GLOBAL.deathCount = 0;
	GLOBAL.stage = 1;

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

	scene.AddObject(new SkyBox, "skybox", LAYER1);
	auto mapObject = scene.AddObject(new Map1, GLOBAL.mapName, LAYER1, true);
	auto centerObject = scene.AddObject(new CenterBuilding(-2.0), "center_building", LAYER1);

	// 떨어지는 바위는 0부터 순차 ID로 생성
	scene.AddObject(new FallingStone(xmfloat3(-114.3, 20.0, -40.9), 0), std::to_string(0), LAYER_STONE);
	scene.AddObject(new FallingStone(xmfloat3(-77.0, 10.0, 71.9), 1), std::to_string(1), LAYER_STONE);
	scene.AddObject(new FallingStone(xmfloat3(34.9, 10.0, -92.9), 2), std::to_string(2), LAYER_STONE);

	// 터레인 유틸 객체와 맵 오브젝트 바운드 데이터를 전역에 저장
	GLOBAL.mapTerrain = mapObject->GetTerrain();
	GLOBAL.mapOOBBdata = mapObject->GetMapWallOOBB();
	GLOBAL.mapOOBBdata.emplace_back(centerObject->GetOOBB());

	if(GLOBAL.skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpawner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(GLOBAL.editMode), "monsterSpawner", LAYER1, GLOBAL.editMode);

	if (GLOBAL.editMode) {
		scene.AddObject(new CameraController(true), "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYER_UI2);
	}
	else {
		if (!GLOBAL.skipDefenseMode) {
			auto block = scene.AddObject(new RoadBlock(XMFLOAT3(-91.0, 5.0, -93.0), 20.0, 10), "roadBlock", LAYER1);

			GLOBAL.mapOOBBdata.emplace_back(block->GetOOBB());

			// map1 몬스터를 20번 스폰하는 디펜스 모드 몬스터 제너레이터
			scene.AddObject(new DefenseModeMonsterGenerator, "defenseModeMonsterGenerator", LAYER1);
		}

		scene.AddObject(new Player1st(GLOBAL.myCharacter), "player", LAYER_PLAYER, true);

	
		if (!GLOBAL.skipTitleMode) {
			GLOBAL.otherIndicator = scene.AddObject(new OtherPlayerIndicator, "otherIndicator", LAYER_UI2);

			for (auto& p : GLOBAL.playerList) {
				std::cout << "currentCharacterType: " << p.second.characterType << std::endl;
				scene.AddObject(new OtherPlayer(p.second.characterType, p.first, p.second.name), std::to_string(p.first), LAYER_PLAYER);
				if (GLOBAL.otherIndicator)
					static_cast<GameObject*>(GLOBAL.otherIndicator)->AddPlayer(p.first, p.second.characterType, p.second.name);
			}
		}

		if(!GLOBAL.skipDefenseMode)
			scene.AddObject(new DefenseIndicator, "map1DefenseIndicator", LAYER_UI2);

		scene.AddObject(new CrossHair, "crosshair", LAYER_UI2, true);
	}

	SendFilePacket(1);
}

// 다른 모드로 전환 시 맵 오브젝트 바운드 데이터 삭제
void Level1::Destructor() {
}