#pragma once
#include "DirectX_3D.h"
#include <random>

class RandomUtil {
private:
	std::default_random_engine dre;

public:
	float Gen(float Min, float Max);
};

extern RandomUtil Random;