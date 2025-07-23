#pragma once
#include "Scene.h"

class DefenseModeMonsterGenerator : public GameObject {
private:
	float       currentTime{};
	float       destGenerateTime{ 2.0 };
	int         currentGenerateCount{};
	int         maxGenerateCount{20};

public:
	DefenseModeMonsterGenerator();
	void InputCreatePositionAndID(float x, float y, unsigned int ID);
	void Update(float Delta) override;
};