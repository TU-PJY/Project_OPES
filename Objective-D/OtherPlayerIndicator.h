#pragma once
#include "Scene.h"
#include "TextUtil.h"

typedef struct {
	int hp;
	std::string name;
}PlayerInfo;

class OtherPlayerIndicator : public GameObject {
private:
	std::unordered_map<unsigned int, PlayerInfo> playerList{};
	Text text{ ALIGN_DEFAULT, HEIGHT_UNDER, XMFLOAT3(1.0, 1.0, 1.0) };

public:
	OtherPlayerIndicator();
	void AddPlayer(unsigned int ID, int chracterType, const std::string& name) override;
	void InputHP(unsigned int ID, int currentHP) override;
	void Update(float Delta) override;
	void Render() override;
};

