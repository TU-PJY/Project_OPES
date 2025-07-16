#pragma once
#include "DirectX_3D.h"
#include <random>

class RandomUtil {
private:
	std::random_device RD;

public:
	float Gen(float Min, float Max);
	XMFLOAT2 GenPointInCircle(float Diameter, const XMFLOAT2& Center);
	XMFLOAT2 GenPointInDonut(float DiameterMin, float DiameterMax, const XMFLOAT2& Center);
};

extern RandomUtil Random;