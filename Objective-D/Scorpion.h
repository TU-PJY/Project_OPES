#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	std::string keyframe{"Idle"};

	TerrainUtil terrainUT{};

	// �Ѿ˿� �ǰݵǾ��ٸ� true�� �Ǿ� �̺�Ʈ �߻�
	// �̺�Ʈ �߻� ���� �ٽ� false�� ��ȯ
	bool hitState{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition);
	bool CheckHit(XMFLOAT2& checkPosition);
	void Update(float Delta);
	void Render();
};

