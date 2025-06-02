#pragma once
#include "GameObject.h"

class BloodParticle {
private:
	std::vector<XMFLOAT3> positions{};
	XMFLOAT3 position{};
	bool render_state{};
	float current_time{};
	float total_time{};

	Vector vec{};

public:
	BloodParticle();
	std::vector<XMFLOAT3> CreatePositions(int count);
	std::vector<XMFLOAT3> GetPositions();
	Vector GetVector();
};
