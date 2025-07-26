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

namespace Level1 { 
	std::deque<GameObject*> ControlObjectList; 
}

void Level1::Start() {
	scene.SetupMode("Level1", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map1";
	GLOBAL.DefenseEnemyRemained = 20;
	GLOBAL.DefenseState = true;

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
	
	// 터레인 유틸 객체와 맵 오브젝트 바운드 데이터를 전역에 저장
	GLOBAL.mapTerrain = mapObject->GetTerrain();
	GLOBAL.mapOOBBdata = mapObject->GetMapWallOOBB();
	GLOBAL.mapOOBBdata.emplace_back(centerObject->GetOOBB());

	if(GLOBAL.skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpawner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(GLOBAL.editMode), "monsterSpawner", LAYER1, GLOBAL.editMode);

	if (GLOBAL.editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}
	else {
		if (!GLOBAL.skipDefenseMode) {
			scene.AddObject(new RoadBlock(XMFLOAT3(-91.0, 5.0, -93.0), 20.0, 10), "roadBlock", LAYER1);

			// map1 몬스터를 20번 스폰하는 디펜스 모드 몬스터 제너레이터
			scene.AddObject(new DefenseModeMonsterGenerator, "defenseModeMonsterGenerator", LAYER1);
		}

		scene.AddObject(new Player1st(CHARACTER_MG), "player", LAYER_PLAYER, true);

	
		if (!GLOBAL.skipTitleMode) {
			GLOBAL.otherIndicator = scene.AddObject(new OtherPlayerIndicator, "otherIndicator", LAYERUI);

			for (auto& p : GLOBAL.playerList) {
				scene.AddObject(new OtherPlayer(CHARACTER_MG, p.first), std::to_string(p.first), LAYER_PLAYER);
				if (GLOBAL.otherIndicator)
					static_cast<GameObject*>(GLOBAL.otherIndicator)->AddPlayer(p.first, CHARACTER_MG, std::to_string(p.first));
			}
		}

		if(!GLOBAL.skipDefenseMode)
			scene.AddObject(new Map1DefenseIndicator, "map1DefenseIndicator", LAYERUI);
	}

}

// 다른 모드로 전환 시 맵 오브젝트 바운드 데이터 삭제
void Level1::Destructor() {
	GLOBAL.mapOOBBdata.clear();
}