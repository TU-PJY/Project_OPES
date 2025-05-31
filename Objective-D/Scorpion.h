#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	std::string keyframe{ "Idle" };

	TerrainUtil terrainUT{};

	// hp 인디케이터
	// 각 몬스터마다 1:1로 배정된다.
	GameObject* hp_ind{};

	FBX fbx{};

	// 총알에 피격되었다면 true가 되어 이벤트 발생
	// 이벤트 발생 직후 다시 false로 전환
	bool hit_state{};

	int full_hp = 100;
	int current_hp = 100;
	int hit_damage{};

	bool death_keyframe_selected{};
	float delete_delay{};

	float start_delay{};
	float curr{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition, float Delay);
	bool CheckHit(XMFLOAT2& checkPosition, int Damage);
	void Update(float Delta);
	void Render();
};

