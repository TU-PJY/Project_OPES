#include "MonsterSpawner.h"
#include "Scorpion.h"
#include "Scene.h"

// �� �ʸ��� �ٸ� ������ ���͸� �������� 50���� ���� ��ȯ�غ���.
// �ϴ��� �� ������ ��ȯ�غ���.
MonsterSpawner::MonsterSpawner(std::string mapName) {
	if (mapName.compare("map1") == 0) {
		scene.AddObject(new Scorpion(mapName, XMFLOAT3(-120.0, 20.0, -90.0)), "scorpion", LAYER1);
	}
}
