#pragma once
#include "Scene.h"

class BuffDebuffIndicator : public GameObject {
private:
	int renderCount{};
	std::deque<bool> buffState{};
	std::deque<bool> debuffState{};

public:
	BuffDebuffIndicator();
	void Render() override;
};

