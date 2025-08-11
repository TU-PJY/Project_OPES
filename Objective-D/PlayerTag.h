#pragma once
#include "Scene.h"
#include "TextUtil.h"

class PlayerTag : public GameObject {
private:
	std::string tag{};
	XMFLOAT3 position{};
	Text     tagText{};

public:
	PlayerTag(const std::string& name);
	void Render() override;
	void InputPosition(XMFLOAT3& Position) override;
};

