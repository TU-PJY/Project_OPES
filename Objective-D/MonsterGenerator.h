#pragma once
#include "Scene.h"

class DefenseModeMonsterGenerator : public GameObject {
private:
	float       currentTime{};
	float       destGenerateTime{ 2.0 };
	int         currentGenerateCount{};
	int         maxGenerateCount{};
	std::string currentMapName{};

public:
	DefenseModeMonsterGenerator(const std::string& mapName, int numMaxGenerate);
	void Update(float Delta) override;
};