#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	std::string keyframe{ "Idle" };

	TerrainUtil terrainUT{};

	// hp �ε�������
	// �� ���͸��� 1:1�� �����ȴ�.
	GameObject* hp_ind{};

	FBX fbx{GlobalSystem, MESH.scorpion};

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
	float curr{};

	XMFLOAT3 destination{ -120.0, 0.0, -120.0 };

	float send_delay{};
	int ID{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition, float Delay, int ID);
	bool CheckHit(XMFLOAT2& checkPosition, int Damage);
	void SendPacket();
	void GiveDamage(int damage);
	int GetID();
	void Update(float Delta);
	void Render();
};

