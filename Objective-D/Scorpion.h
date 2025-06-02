#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"
#include "MuzzleFlash.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	float rotation{};
	float dest_rotation{};
	std::string keyframe{ "Idle" };

	TerrainUtil terrainUT{};

	// hp 인디케이터
	// 각 몬스터마다 1:1로 배정된다.
	GameObject* hp_ind{};

	FBX fbx{ GlobalSystem, MESH.scorpion };

	// 총알에 피격되었다면 true가 되어 이벤트 발생
	// 이벤트 발생 직후 다시 false로 전환
	bool hit_state{};

	int full_hp = 100;
	int current_hp = 100;
	int prev_hp = 100;
	int hit_damage{};

	bool death_keyframe_selected{};
	float delete_delay{};

	float start_delay{};
	float current_delay{};

	XMFLOAT3 destination{ -120.0, 0.0, -120.0 };

	float send_delay{};
	int ID{};

	bool activate_state{};
	bool move_state{ true };
	bool avoid_state{};
	float avoid_time{};
	int avoid_dir{};

	bool idle_selected{}, move_selected{}, attack_selected{};
	bool attack_state{};

	OOBB oobb{};

	Vector vec{};

	BloodParticle blood{};
	float particle_alpha{};
	float particle_time{};
	bool render_particle{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition, float Delay, int ID);
	bool CheckHit(XMFLOAT2& checkPosition, int Damage);
	void SendPacket(float Delta);
	bool ChangeHP(int damage);
	int GetID();
	OOBB GetOOBB();
	XMFLOAT3 GetPosition();
	bool GetDeathState();
	void Update(float Delta);
	void Render();
};

