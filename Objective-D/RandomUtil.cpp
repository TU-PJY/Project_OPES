#include "RandomUtil.h"

RandomUtil Random;

float RandomUtil::Gen(float Min, float Max) {
    static std::default_random_engine dre(RD());
	std::uniform_real_distribution urd{ Min, Max };
	return urd(dre);
}

int RandomUtil::Gen(int Min, int Max) {
    static std::default_random_engine dre(RD());
    std::uniform_int_distribution uid{ Min, Max };
    return uid(dre);
}


XMFLOAT2 RandomUtil::GenPointInCircle(float Diameter, const XMFLOAT2& Center) {
    static std::default_random_engine dre(RD());
    static std::uniform_real_distribution<float> DistAngle(0.0f, 2.0f * XM_PI);
    static std::uniform_real_distribution<float> DistRadius(0.0f, 1.0f);

    float Angle = DistAngle(dre);
    float R = Diameter * 0.5 * std::sqrt(DistRadius(dre));

    float X = R * std::cosf(Angle) + Center.x;
    float Y = R * std::sinf(Angle) + Center.y;

    return XMFLOAT2(X, Y);
}

XMFLOAT2 RandomUtil::GenPointInDonut(float DiameterMin, float DiameterMax, const XMFLOAT2& Center) {
    static std::default_random_engine dre(RD());
    static std::uniform_real_distribution<float> DistAngle(0.0f, 2.0f * XM_PI);
    static std::uniform_real_distribution<float> DistRadius(0.0f, 1.0f);

    float Angle = DistAngle(dre);
    float RadiusMin = DiameterMin * 0.5;
    float RadiusMax = DiameterMax * 0.5;
    float R = std::sqrt((RadiusMax * RadiusMax - RadiusMin * RadiusMin) * DistRadius(dre) + RadiusMin * RadiusMin);

    float X = R * std::cosf(Angle) + Center.x;
    float Y = R * std::sinf(Angle) + Center.y;
    return XMFLOAT2(X, Y);
}