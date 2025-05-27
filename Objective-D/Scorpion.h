#pragma once
#include "GameObject.h"
#include "TerrainUtil.h"

class Scorpion : public GameObject {
private:
	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	std::string keyframe{"Idle"};

	TerrainUtil terrainUT{};

	// 총알에 피격되었다면 true가 되어 이벤트 발생
	// 이벤트 발생 직후 다시 false로 전환
	bool hitState{};
	
public:
	Scorpion(std::string mapName, XMFLOAT3& createPosition);
	bool CheckHit(XMFLOAT2& checkPosition);
	void Update(float Delta);
	void Render();
};

