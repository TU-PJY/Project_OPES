#include "ClampUtil.h"

// CLAMP_FIX: 대상 값이 최대 값과 최소 값 사이의 값을 유지한다.
// CLAMP_RETURN: 대상 값이 최대 값을 초과하면 최소 값으로, 최소 값을 초과하면 최대 값으로 변경한다.
void Clamp::ClampValue(float& Value, float Min, float Max, int ClampFlag) {
	if (Value > Max) {
		if (ClampFlag == CLAMP_FIX)
			Value = Max;
		else if (ClampFlag == CLAMP_RETURN)
			Value = Min;
	}
	if (Value < Min) {
		if (ClampFlag == CLAMP_FIX)
			Value = Min;
		else if (ClampFlag == CLAMP_RETURN)
			Value = Max;
	}
}

void Clamp::ClampValue(int& Value, int Min, int Max, int ClampFlag) {
	if (Value > Max) {
		if (ClampFlag == CLAMP_FIX)
			Value = Max;
		else if (ClampFlag == CLAMP_RETURN)
			Value = Min;
	}
	if (Value < Min) {
		if (ClampFlag == CLAMP_FIX)
			Value = Min;
		else if (ClampFlag == CLAMP_RETURN)
			Value = Max;
	}
}

// CLAMP_DIR_LESS: 제한할 값이 대상 값보다 작아야 하는 경우
// CLAMP_DIR_GREATER: 제한할 값이 대상 값보다 커야하는 경우
void Clamp::LimitValue(float& Value, float LimitValue, int ValueChangeDirection) {
	if (ValueChangeDirection == CLAMP_DIR_LESS) {
		if (Value < LimitValue)
			Value = LimitValue;
	}
	else if (ValueChangeDirection == CLAMP_DIR_GREATER) {
		if (Value > LimitValue)
			Value = LimitValue;
	}
}

void Clamp::LimitValue(int& Value, int LimitValue, int ValueChangeDirection) {
	if (ValueChangeDirection == CLAMP_DIR_LESS) {
		if (Value < LimitValue)
			Value = LimitValue;
	}
	else if (ValueChangeDirection == CLAMP_DIR_GREATER) {
		if (Value > LimitValue)
			Value = LimitValue;
	}
}
