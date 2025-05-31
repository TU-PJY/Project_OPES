#pragma once
#include "GameObject.h"

class MonsterSpawner : public GameObject {
public:
	std::string map_name{};
	MonsterSpawner(std::string mapName);
	void Update(float Delta);
};