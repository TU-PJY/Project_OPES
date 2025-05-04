#pragma once
#include "DirectX_3D.h"

namespace Clamp {
	void ClampValue(float& Value, float Min, float Max, int ClampFlag);
	void ClampValue(int& Value, int Min, int Max, int ClampFlag);
	void LimitValue(float& Value, float LimitValue, int ValueChangeDirection);
	void LimitValue(int& Value, int LimitValue, int ValueChangeDirection);
}