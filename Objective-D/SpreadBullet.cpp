#include "SpreadBullet.h"
#include "MathUtil.h"

void SendPtoMDamagePacket(unsigned int monsterID, int attackHp);

SpreadBullet::SpreadBullet(int damage) {
	this->damage = damage;

	//std::cout << "spread bullet created" << std::endl;
}

void SpreadBullet::Update(float Delta) {
	Math::GenRandomSpreadingRaysFromCenter(rays, SG_FRAG, 3.0);

	size_t size = scene.LayerSize(LAYER_MONSTER);
	float distance;

	for (auto& ray : rays) {
		RayTarget newTarget{};

		// 맵 오브젝트와 충돌
		for (auto& o : GLOBAL.mapOOBBdata) {
			if (o.oobb.Intersects(ray.first, ray.second, distance)) {
				newTarget.Add(nullptr, distance);
			}
		}

		// 지형과 충돌
		xmfloat3 result = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, ray.first, ray.second, distance);
		if (distance > 0.0)
			newTarget.Add(nullptr, distance);

		// 몬스터와 충돌
		for (int i = 0; i < size; i++) {
			float distance;
			if (auto monster = scene.ReferLayer(LAYER_MONSTER, i); monster) {
				if (monster->CheckHit(ray.first, ray.second, distance))
					newTarget.Add(monster, distance);
			}
		}

		targets.emplace_back(newTarget);
	}

	for (auto& t : targets) {
		auto target = t.GetNearestTarget();
		if (!damageList.contains(target))
			damageList.emplace(target, damage);
		else
			damageList[target] += damage;
	}

	/*for (auto& d : damageList) {
		if (d.first) {
			if (!GLOBAL.useServer)
				d.first->GiveDamage(d.second);
			else
				SendPtoMDamagePacket(d.first->GetID(), d.second);
		}
	}*/

	if (!damageList.empty()) {
		auto It = damageList.begin();
		if (It->first) {
			if (!GLOBAL.useServer)
				It->first->GiveDamage(It->second);
			else
				SendPtoMDamagePacket(It->first->GetID(), It->second);
		}
		damageList.erase(It);
	}

	if(damageList.empty())
		scene.DeleteObject(this);
}
