#pragma once
#include "DirectX_3D.h"
#include "ClampUtil.h"
#include <type_traits>

// 레벨 시작 시 재생되는 연출에 사용될 애니메이션 모듈.

namespace PRESET {
	constexpr float ZERO_INCREASE_ZERO     = 0.f;
	constexpr float ZERO_INCREASE_NEGATIVE = -XM_PI * 2.f;
	constexpr float ZERO_INCREASE_POSITIVE = XM_PI * 2.f;
	constexpr float ZERO_DECREASE_NEGATIVE = -XM_PI;
	constexpr float ZERO_DECREASE_POSITIVE = XM_PI;
	constexpr float MAX_NEGATIVE           = -3.f * XM_PI / 2.f;
	constexpr float MAX_POSITIVE           = XM_PI / 2.f;
	constexpr float MIN_NEGATIVE           = -XM_PI / 2.f;
	constexpr float MIN_POSITIVE           = 3.f * XM_PI / 2.f;
}

class SinMove {
private:
	float CurrentValue{ PRESET::MIN_NEGATIVE };

public:
	template <typename Type>
	void Update(Type& Value, const Type& StartPoint, const Type& EndPoint, float Speed, float Delta) {
		CurrentValue += Delta * Speed;
		Clamp::ClampValue(CurrentValue, PRESET::MIN_NEGATIVE, PRESET::MAX_POSITIVE, CLAMP_FIX);

		if constexpr (std::is_same_v<Type, float>)
			Value = CalcSinPosition(CurrentValue, StartPoint, EndPoint);

		else if constexpr (std::is_same_v<Type, XMFLOAT3>) {
			Value.x = CalcSinPosition(CurrentValue, StartPoint.x, EndPoint.y);
			Value.y = CalcSinPosition(CurrentValue, StartPoint.y, EndPoint.y);
			Value.z = CalcSinPosition(CurrentValue, StartPoint.z, EndPoint.z);
		}
	}

	void Reset();

private:
	float CalcSinPosition(float Num, float Start, float End);
};

class EaseInOut {
private:
	float CurrentTime{};

public:
	template <typename Type>
	void Update(Type& Value, const Type& StartPoint, const Type& EndPoint, float Speed, float Delta) {
		CurrentTime += Delta * Speed;
		Clamp::ClampValue(CurrentTime, 0.f, 1.f, CLAMP_FIX);

		if constexpr (std::is_same_v<Type, float>)
			Value = CalcEaseInOut(CurrentTime, StartPoint, EndPoint, 10.f);

		else if constexpr (std::is_same_v<Type, XMFLOAT3>) {
			Value.x = CalcEaseInOut(CurrentTime, StartPoint.x, EndPoint.x, 10.f);
			Value.y = CalcEaseInOut(CurrentTime, StartPoint.y, EndPoint.y, 10.f);
			Value.z = CalcEaseInOut(CurrentTime, StartPoint.z, EndPoint.z, 10.f);
		}
	}

	void Reset();

private:
	float CalcEaseInOut(float t, float start, float end, float Intensity);
};