#pragma once
#include "Scene.h"
#include "TextUtil.h"

class PlayerTag : public GameObject {
private:
	std::string tag{};
	XMFLOAT3 position{};
	Text     tagText{};

	std::deque<bool> buffState{};
	std::deque<bool> debuffState{};

public:
	PlayerTag(const std::string& name);
	void SetBuffDebuff(unsigned int ID) override;
	void Render() override;
	void InputPosition(XMFLOAT3& Position) override;
};

