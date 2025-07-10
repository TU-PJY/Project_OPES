#include "RandomUtil.h"

RandomUtil Random;

float RandomUtil::Gen(float Min, float Max) {
	std::uniform_real_distribution urd{ Min, Max };
	return urd(dre);
}
