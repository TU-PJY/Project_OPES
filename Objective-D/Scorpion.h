#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	std::string keyframe{"Idle"};

	TerrainUtil terrainUT{};

	// hp �ε�������
	// �� ���͸��� 1:1�� �����ȴ�.
	GameObject* hp_ind{};

	// �Ѿ˿� �ǰݵǾ��ٸ� true�� �Ǿ� �̺�Ʈ �߻�
	// �̺�Ʈ �߻� ���� �ٽ� false�� ��ȯ
	bool hit_state{};

	int full_hp = 100;
	int current_hp = 100;
	int hit_damage{};

	bool death_keyframe_selected{};
	float delete_delay{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition);
	bool CheckHit(XMFLOAT2& checkPosition, int Damage);
	void Update(float Delta);
	void Render();
};

