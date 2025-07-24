#include "MonsterGenerator.h"
#include "RandomUtil.h"
#include "PlantMonster.h"


DefenseModeMonsterGenerator::DefenseModeMonsterGenerator() {
	maxGenerateCount = GLOBAL.map1DefenseEnemyRemained;
}

// GLOBAL.useServer가 true일 경우 서버로부터 패킷을 받아 그 자리에 생성한다.
void DefenseModeMonsterGenerator::InputCreatePositionAndID(float x, float z, unsigned int ID) {
	if (!GLOBAL.useServer)
		return;

	xmfloat3 createPosition = xmfloat3(x, 0.0, z);
	scene.AddObject(new PlantMonster(createPosition, ID, true), std::to_string(ID), LAYER_MONSTER);
}

// GLOBAL,useServer가 true일 경우 아래 함수는 동작하지 않는다.
void DefenseModeMonsterGenerator::Update(float Delta) {
	if (GLOBAL.useServer)
		return;

	currentTime += Delta;

	if (currentTime >= destGenerateTime) {
		currentTime -= destGenerateTime;
		XMFLOAT3 randomPosition{};
		XMFLOAT2 RandomXZ = Random.GenPointInDonut(30.0, 60.0, XMFLOAT2(-120.0, -120.0));

		// 위치 랜덤
		randomPosition.x = RandomXZ.x;
		randomPosition.z = RandomXZ.y;

		if (GLOBAL.mapName.compare("map1") == 0) {
			scene.AddObject(new PlantMonster(randomPosition, currentID, true), std::to_string(currentID), LAYER_MONSTER);
			std::cout << currentGenerateCount << "번째 생성" << std::endl;
		}

		currentGenerateCount++;
		currentID++;

		// 최대 스폰 횟수에 도달하면 스스로 삭제하여 몬스터 스폰을 중단한다.
		if (currentGenerateCount == maxGenerateCount) {
			scene.DeleteObject(this);
			std::cout << "제너레이터 삭제됨" << std::endl;
		}
	}
}
