#include "MonsterSpawner.h"
#include "PlantMonster.h"
#include "Scorpion.h"

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
		if (GLOBAL.mapName.compare("map1") == 0) {
			scene.DeleteObject("plantMonster", DELETE_RANGE_ALL);
			scene.DeleteObject("scorpion", DELETE_RANGE_ALL);
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
				scene.AddObject(new PlantMonster(position[i], currentID, false), "plantMonster", LAYER2);
			if(type[i] == 2)
				scene.AddObject(new Scorpion(position[i], currentID), "scorpion", LAYER2);

			currentID++;
		}
	}
}

// 디펜스 모드에서 모든 몬스터가 죽으면 어드벤처모드 몬스터를 스폰한 후 삭제된다.
// 에디트 모드에서는 F5를 누를떄마다 새로 스폰한다.
void MonsterSpawner::Update(float Delta) {
	if (editMode)
		return;
	
	if (GLOBAL.mapName.compare("map1") == 0 && GLOBAL.map1DefenseEnemyRemained == 0) {
		LoadDataAndSpawnMonster();
		scene.DeleteObject(this);
	}
}