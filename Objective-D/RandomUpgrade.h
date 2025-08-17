#pragma once
#include "Scene.h"

class RandomUpgrade : public GameObject {
private:
	float cardSize{ 1.2 };
	bool randomCreated{};
	float opacity{};
	float delay{};

	int buffResult{};
	int debuffResult{};

public:
	RandomUpgrade();
	void GetRandomBuff();
	void GetRandomDebuff();
	void Update(float delta) override;
	void Render() override;
};

