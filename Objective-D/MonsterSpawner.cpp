#include "MonsterSpawner.h"
#include "Scorpion.h"
#include "Scene.h"
#include <random>

// 각 맵마다 다른 종류의 몬스터를 무작위로 50마리 정도 소환해본다.
// 일단은 한 종류만 소환해본다.
MonsterSpawner::MonsterSpawner(std::string mapName) {
	map_name = mapName;
}


void MonsterSpawner::Update(float Delta) {
	std::default_random_engine dre;
	std::uniform_real_distribution urd{ -10.0, 10.0 };

	for (int i = 0; i < 5; i++) {
		if (map_name.compare("map1") == 0) {
			XMFLOAT3 spawn_pos = XMFLOAT3(-120.0 + (float)urd(dre), 20.0, -90.0 + (float)urd(dre));
			scene.AddObject(new Scorpion(map_name, spawn_pos, 0.5 * i), "scorpion", LAYER1);
		}
	}

	scene.DeleteObject(this);
}