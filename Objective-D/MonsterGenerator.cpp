#include "MonsterGenerator.h"
#include "RandomUtil.h"
#include "PlantMonster.h"


DefenseModeMonsterGenerator::DefenseModeMonsterGenerator() {
	maxGenerateCount = GLOBAL.map1DefenseEnemyRemained;
}

void DefenseModeMonsterGenerator::Update(float Delta) {
	currentTime += Delta;

	if (currentTime >= destGenerateTime) {
		currentTime -= destGenerateTime;
		XMFLOAT3 randomPosition{};
		XMFLOAT2 RandomXZ = Random.GenPointInDonut(30.0, 60.0, XMFLOAT2(-120.0, -120.0));

		// 위치 랜덤
		randomPosition.x = RandomXZ.x;
		randomPosition.z = RandomXZ.y;

		if (GLOBAL.mapName.compare("map1") == 0)
			scene.AddObject(new PlantMonster(randomPosition, 0, true), "plantMonster", LAYER_MONSTER);

		currentGenerateCount++;

		// 최대 스폰 횟수에 도달하면 스스로 삭제하여 몬스터 스폰을 중단한다.
		if (currentGenerateCount == maxGenerateCount)
			scene.DeleteObject(this);
	}
}
