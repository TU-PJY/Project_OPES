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
	XMFLOAT3 spawn_pos1 = XMFLOAT3(-60.0, -20.0, -70.0);
	XMFLOAT3 spawn_pos2 = XMFLOAT3(-80.0, -20.0, -60.0);
	XMFLOAT3 spawn_pos3 = XMFLOAT3(-100.0, -20.0, -60.0);

	float delay = 0.5;
	int id = 1;

	for (int i = 0; i < 5; i++) {
		if (map_name.compare("map1") == 0) {
			scene.AddObject(new Scorpion(map_name, spawn_pos1, delay, id), "scorpion", LAYER1);
			delay += 0.5;
			id++;
			scene.AddObject(new Scorpion(map_name, spawn_pos2, delay, id), "scorpion", LAYER1);
			delay += 0.5;
			id++;
			scene.AddObject(new Scorpion(map_name, spawn_pos3, delay, id), "scorpion", LAYER1);
			delay += 0.5;
			id++;
		}
	}

	scene.DeleteObject(this);
}