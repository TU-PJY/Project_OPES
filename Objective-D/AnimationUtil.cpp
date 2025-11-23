#include "AnimationUtil.h"

void SinMove::Reset() {
	CurrentValue = PRESET::MIN_NEGATIVE;
}

float SinMove::CalcSinPosition(float Num, float Start, float End) {
	return Start + (sinf(Num) - sinf(PRESET::MIN_NEGATIVE)) * (End - Start) * 0.5f;
}

float EaseInOut::CalcEaseInOut(float Time, float StartPoint, float EndPoint, float Intensity) {
	float Change = EndPoint - StartPoint;
	Time *= 2.f;
	if (Time < 1.f)
		return Change / 2.f * pow(2.f, Intensity * (Time - 1.f)) + StartPoint;
	Time -= 1.f;
	return Change / 2.f * (-pow(2.f, -Intensity * Time) + 2.f) + StartPoint;
}
