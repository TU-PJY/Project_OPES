#pragma once
#include "Scene.h"

class DefenseModeMonsterGenerator : public GameObject {
private:
	float       currentTime{};
	float       destGenerateTime{ 1.0 };
	int         currentGenerateCount{};
	int         maxGenerateCount{20};
	unsigned int currentID{};

public:
	DefenseModeMonsterGenerator();
	void InputCreatePositionAndID(float x, float y, unsigned int ID);
	void Update(float Delta) override;
};