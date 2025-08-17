#pragma once
#include "Scene.h"
#include "TextUtil.h"

class RandomUpgrade : public GameObject {
private:
	float cardSize{ 1.2 };
	bool randomCreated{};
	float opacity{};
	float delay{};

	int buffResult{};
	int debuffResult{};

	Text text{};

public:
	RandomUpgrade();
	void GetRandomBuff();
	void GetRandomDebuff();
	void Update(float delta) override;
	void Render() override;
};

