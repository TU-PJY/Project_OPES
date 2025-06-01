#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	float rotation{};
	float dest_rotation{};
	std::string keyframe{ "Idle" };

	TerrainUtil terrainUT{};

	// hp �ε�������
	// �� ���͸��� 1:1�� �����ȴ�.
	GameObject* hp_ind{};

	FBX fbx{ GlobalSystem, MESH.scorpion };

	// �Ѿ˿� �ǰݵǾ��ٸ� true�� �Ǿ� �̺�Ʈ �߻�
	// �̺�Ʈ �߻� ���� �ٽ� false�� ��ȯ
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

	OOBB oobb{};

	Vector vec{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition, float Delay, int ID);
	bool CheckHit(XMFLOAT2& checkPosition, int Damage);
	void SendPacket(float Delta);
	void ChangeHP(int damage);
	int GetID();
	OOBB GetOOBB();
	XMFLOAT3 GetPosition();
	bool GetDeathState();
	void Update(float Delta);
	void Render();
};

