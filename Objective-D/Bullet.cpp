#include "Bullet.h"
#include "MathUtil.h"
#include "CameraUtil.h"
#include "ClampUtil.h"
#include "PickingUtil.h"

Bullet::Bullet(int damage) {
	bulletDamage = damage;
}

void Bullet::updateCollision() {
	// 먼저 터레인과 맵 오브젝트에 광선이 충돌하는지 검사 후, 충돌하면 충돌 타겟 후보에 추가한다.

	float terrainDistance{};
	XMFLOAT3 pickPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, terrainDistance);

	// 실제로 충돌 했을 때만 추가
	if(terrainDistance > 0.0)
		rayTarget.Add(nullptr, terrainDistance);

	float mapObjectDistance{};
	for (auto& B : GLOBAL.mapOOBBdata) {
		if (PickingUtil::PickByViewportOOBB(XMFLOAT2(0.0, 0.0), mapObjectDistance, B)) {
			rayTarget.Add(nullptr, mapObjectDistance);
			break;
		}
	}

	// 그 다음 몬스터들이 광선과 충돌하면 충돌 타켓 후보에 추가한다.
	if (GLOBAL.mapName.compare("map1") == 0) {
		size_t size = scene.LayerSize(LAYER_MONSTER);
		for (int i = 0; i < size; i++) {
			if (auto object = scene.ReferLayer(LAYER_MONSTER, i); object) {
				float distance{};
				if (object->CheckHit(distance)) 
					rayTarget.Add(object, distance);
			}
		}
	}

	// 가장 가까운 거리를 가지는 타겟이 지형이나 맵 오브젝트라면 nullptr을 리턴하여 어떠한 몬스터도 대미지를 입지 않게 된다.
	auto ptr = rayTarget.GetNearestTarget();
	if (ptr)
		ptr->GiveDamage(bulletDamage);

	// 최종적으로 삭제
	scene.DeleteObject(this);
}

void Bullet::Update(float Delta) {
	updateCollision();
}