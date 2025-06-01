#include "MonsterSpawner.h"
#include "Scorpion.h"
#include "Scene.h"
#include <random>

// �� �ʸ��� �ٸ� ������ ���͸� �������� 50���� ���� ��ȯ�غ���.
// �ϴ��� �� ������ ��ȯ�غ���.
MonsterSpawner::MonsterSpawner(std::string mapName) {
	map_name = mapName;
}


void MonsterSpawner::Update(float Delta) {
	std::default_random_engine dre;
	std::uniform_real_distribution urd{ -10.0, 10.0 };
	XMFLOAT3 spawn_pos1 = XMFLOAT3(-60.0, 20.0, -70.0);
	XMFLOAT3 spawn_pos2 = XMFLOAT3(-80.0, 20.0, -60.0);
	XMFLOAT3 spawn_pos3 = XMFLOAT3(-100.0, 20.0, -60.0);

	//for (int i = 0; i < 10; i++) {
		if (map_name.compare("map1") == 0) {
			scene.AddObject(new Scorpion(map_name, spawn_pos1, 0.0), "0", LAYER1);
			scene.AddObject(new Scorpion(map_name, spawn_pos2, 0.0), "1", LAYER1);
			scene.AddObject(new Scorpion(map_name, spawn_pos3, 0.0), "2", LAYER1);
		}
	//}

	scene.DeleteObject(this);
}