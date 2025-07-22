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

namespace Level1 { 
	std::deque<GameObject*> ControlObjectList; 
}

bool editMode = false;

// 활성화 시 디펜스 모드 건너뜀
bool skipDefenseMode = true;

void Level1::Start() {
	scene.SetupMode("Level1", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map1";
	GLOBAL.map1DefenseEnemyRemained = 20;
	GLOBAL.map1DefenseState = true;

	scene.AddObject(new SkyBox, "skybox", LAYER1);
	auto mapObject = scene.AddObject(new Map1, GLOBAL.mapName, LAYER1, true);
	auto centerObject = scene.AddObject(new CenterBuilding(-2.0), "center_building", LAYER1);
	
	// 터레인 유틸 객체와 맵 오브젝트 바운드 데이터를 전역에 저장
	GLOBAL.mapTerrain = mapObject->GetTerrain();
	GLOBAL.mapOOBBdata = mapObject->GetMapWallOOBB();
	GLOBAL.mapOOBBdata.emplace_back(centerObject->GetOOBB());

	if(skipDefenseMode)
		scene.AddObject(new MonsterSpawner(true), "monsterSpawner", LAYER1, true);
	else
		scene.AddObject(new MonsterSpawner(editMode), "monsterSpawner", LAYER1, editMode);

	if (editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}
	else {
		if (!skipDefenseMode) {
			scene.AddObject(new RoadBlock(XMFLOAT3(-91.0, 5.0, -93.0), 20.0, 10), "roadBlock", LAYER1);

			// map1 몬스터를 20번 스폰하는 디펜스 모드 몬스터 제너레이터
			scene.AddObject(new DefenseModeMonsterGenerator, "defenseModeMonsterGenerator", LAYER1);
		}

		scene.AddObject(new Player1st(CHARACTER_MG), "player", LAYER_PLAYER, true);

		if(!skipDefenseMode)
			scene.AddObject(new Map1DefenseIndicator, "map1DefenseIndicator", LAYERUI);
	}

}

// 다른 모드로 전환 시 맵 오브젝트 바운드 데이터 삭제
void Level1::Destructor() {
	GLOBAL.mapOOBBdata.clear();
}