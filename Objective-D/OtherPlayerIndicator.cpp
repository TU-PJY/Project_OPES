#include "OtherPlayerIndicator.h"

OtherPlayerIndicator::OtherPlayerIndicator() {
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.01, -0.01), 0.5);
}

void OtherPlayerIndicator::AddPlayer(unsigned int ID, int chracterType, const std::string& name) {
	PlayerInfo newInfo{};
	newInfo.name = name;

	switch (chracterType) {
	case CHARACTER_MG:
		newInfo.hp = 100;
		break;

	case CHARACTER_DMR:
		newInfo.hp = 100;
		break;

	case CHARACTER_ENG:
		newInfo.hp = 100;
		break;
	}

	playerList.emplace(ID, newInfo);
}

void OtherPlayerIndicator::InputHP(unsigned int ID, int currentHP) {
	auto found = playerList.find(ID);
	if (found != playerList.end())
		found->second.hp = currentHP;
}

void OtherPlayerIndicator::Update(float Delta) {

}

void OtherPlayerIndicator::Render() {
	int currentIndex{};
	for (auto& p : playerList) {
		std::string renderStr = p.second.name + "(ID: " + std::to_string(p.first) + ") " + std::to_string(p.second.hp);
		text.Render(XMFLOAT2(-1.0 * ASPECT + 0.05, 1.0 - currentIndex * 0.1), 0.1, renderStr);
		currentIndex++;
	}
}
