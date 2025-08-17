#pragma once
#include "Scene.h"

class BuffDebuffIndicator : public GameObject {
private:
	int buffCount{};
	int deBuffCount{};
	int renderCount{};

	std::deque<float> buffPosition{};
	std::deque<float> deBuffPosition{};

public:
	BuffDebuffIndicator();
	void Render() override;
};

