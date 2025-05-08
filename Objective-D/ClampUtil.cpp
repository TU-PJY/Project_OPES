#include "ClampUtil.h"

// CLAMP_FIX: ��� ���� �ִ� ���� �ּ� �� ������ ���� �����Ѵ�.
// CLAMP_RETURN: ��� ���� �ִ� ���� �ʰ��ϸ� �ּ� ������, �ּ� ���� �ʰ��ϸ� �ִ� ������ �����Ѵ�.
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

// CLAMP_DIR_LESS: ������ ���� ��� ������ �۾ƾ� �ϴ� ���
// CLAMP_DIR_GREATER: ������ ���� ��� ������ Ŀ���ϴ� ���
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
