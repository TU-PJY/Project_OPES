#pragma once
#include "Scene.h"

class BuffDebuffIndicator : public GameObject {
private:
	int buffCount{};
	int deBuffCount{};
	std::deque<float> buffPosition{};
	std::deque<float> deBuffPosition{};

public:
	BuffDebuffIndicator();
	void Render() override;
};

