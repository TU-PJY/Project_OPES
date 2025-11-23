#include "MonsterSpawner.h"
#include "PlantMonster.h"
#include "Scorpion.h"
#include "Troll.h"
#include "Treant.h"
#include "Imp.h"
#include "Gazer.h"

void SendFilePacket(int stage, bool startDefense);

// 현재 맵에 따라 다른 몬스터 데이터를 로드하도록 한다.
MonsterSpawner::MonsterSpawner(bool editMode) {
	this->editMode = editMode;

	if(this->editMode)
		LoadDataAndSpawnMonster();
}

// F5를 누르면 기존 몬스터들을 모두 제거 후 수정된 데이터를 로드한 후 몬스터를 스폰한다.
// editMode에서만 동작한다.
void MonsterSpawner::InputKey(KeyEvent& Event) {
	if (!editMode)
		return;

	if (Event.Type == WM_KEYDOWN && Event.Key == VK_F5) {
		size_t size = scene.LayerSize(LAYER_MONSTER);
		for (int i = 0; i < size; i++) {
			if (auto monster = scene.ReferLayer(LAYER_MONSTER, i); monster)
				scene.DeleteObject(monster);
		}

		LoadDataAndSpawnMonster();
	}
}

void MonsterSpawner::LoadDataAndSpawnMonster() {
	// 현재 맵에 따라 다른 데이터를 로드한다.
	script.Release();
	position.clear();
	type.clear();
	currentCreateDelay = 0.0;

	if (GLOBAL.mapName.compare("map1") == 0)
		script.Load("Resources//Scripts//map1//map1-monster.xml");

	else if(GLOBAL.mapName.compare("map2") == 0)
		script.Load("Resources//Scripts//map2//map2-monster.xml");

	else if (GLOBAL.mapName.compare("map3") == 0)
		script.Load("Resources//Scripts//map3//map3-monster.xml");

	auto custumLoad = [&](CategoryPtr Category)
	{
		int loadedType;
		XMFLOAT3 loadedPosition;

		loadedType = (int)script.GetDigitData(Category, "type");
		loadedPosition.x = script.GetDigitData(Category, "x");
		loadedPosition.z = script.GetDigitData(Category, "z");
		loadedPosition.y = 0.0; // 몬스터 높이는 몬스터 객체가 알아서 맞춤

		position.emplace_back(loadedPosition);
		type.emplace_back(loadedType);
	};
	
	// 람다로 정의한 동작대로 로드 수행
	script.LoadAllData(custumLoad);

	size_t size = position.size();

	unsigned int currentID{};

	if (GLOBAL.mapName.compare("map1") == 0) {
		for (int i = 0; i < size; i++) {
			if (type[i] == 1)
				scene.AddObject(new PlantMonster(position[i], currentID, false), std::to_string(currentID), LAYER_MONSTER);
			else if(type[i] == 2)
				scene.AddObject(new Scorpion(position[i], currentID), std::to_string(currentID), LAYER_MONSTER);

			currentID++;
		}
	}

	else if (GLOBAL.mapName.compare("map2") == 0) {
		for (int i = 0; i < size; i++) {
			if (type[i] == 1)
				scene.AddObject(new Troll(position[i], currentID), std::to_string(currentID), LAYER_MONSTER);
			else if (type[i] == 2)
				scene.AddObject(new Treant(position[i], currentID, false), std::to_string(currentID), LAYER_MONSTER);

			currentID++;
		}
	}

	else if (GLOBAL.mapName.compare("map3") == 0) {
		for (int i = 0; i < size; i++) {
			if (type[i] == 1)
				scene.AddObject(new Imp(position[i], currentID), std::to_string(currentID), LAYER_MONSTER);
			else if (type[i] == 2)
				scene.AddObject(new Gazer(position[i], currentID), std::to_string(currentID), LAYER_MONSTER);

			currentID++;
		}
	}
}

// 디펜스 모드에서 모든 몬스터가 죽으면 어드벤처모드 몬스터를 스폰한 후 삭제된다.
// 에디트 모드에서는 F5를 누를떄마다 새로 스폰한다.
void MonsterSpawner::Update(float Delta) {
	if (editMode)
		return;
	
	if (GLOBAL.mapName == "map1" && GLOBAL.Map1DefenseEnemyRemained == 0) {
		LoadDataAndSpawnMonster();
		SendFilePacket(1, false);
		scene.DeleteObject(this);
	}

	if (GLOBAL.mapName == "map2" && GLOBAL.Map2DefenseEnemyRemained == 0) {
		LoadDataAndSpawnMonster();
		SendFilePacket(2, false);
		scene.DeleteObject(this);
	}

	if (GLOBAL.mapName == "map3" && GLOBAL.Map3DefenseEnemyRemained == 0) {
		LoadDataAndSpawnMonster();
		SendFilePacket(3, false);
		scene.DeleteObject(this);
	}
}