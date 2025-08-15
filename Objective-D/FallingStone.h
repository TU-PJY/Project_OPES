#pragma once
#include "Scene.h"
#include "TerrainUtil.h"

class FallingStone : public GameObject {
private:
	float fallingAcc{};
	xmfloat3 position{};
	xmfloat3 rotation{};
	xmfloat3 size{};
	TerrainUtil terrain{};

	OOBB oobb{};
	BoundSphere sphere{};

	// hp가 0이 되어야 떨어짐
	int hp{100}; 

	// 클라 - 서버 동기화를 위한 ID
	unsigned int ID{};

public:
	FallingStone(const xmfloat3& createPosition, unsigned int createID);
	void Update(float Delta) override;
	void Render() override;
	OOBB GetOOBB() override;
	bool CheckHit(float& distance) override;
	bool CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) override;
	unsigned int GetID() override;
	void GiveDamage(int damage) override;
	void InputHP(int hp) override;
};


