#pragma once
#include "Scene.h"

class BuffDebuffIndicator : public GameObject {
private:
	int buffCount{};
	int deBuffCount{};
	std::vector<float> buffPosition{};
	std::vector<float> deBuffPosition{};

public:
	BuffDebuffIndicator();
	void Render() override;
};

