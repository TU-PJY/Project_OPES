#include "MonsterSpawner.h"
#include "Scorpion.h"
#include "Scene.h"

// 각 맵마다 다른 종류의 몬스터를 무작위로 50마리 정도 소환해본다.
// 일단은 한 종류만 소환해본다.
MonsterSpawner::MonsterSpawner(std::string mapName) {
	if (mapName.compare("map1") == 0) {
		scene.AddObject(new Scorpion(mapName, XMFLOAT3(-120.0, 20.0, -90.0)), "scorpion", LAYER1);
	}
}
