#pragma once
#include "DirectX_3D.h"

class LineBrush {
public:
	XMFLOAT4X4 TranslateMatrix{};
	XMFLOAT4X4 ScaleMatrix{};
	XMFLOAT3 LineColor{};
	float TransparencyValue{ 1.0f };

	float Length{}, Rotation{};

	// 선의 색상을 지정한다.
	void SetColor(float R, float G, float B);
	void Draw(float X1, float Y1, float X2, float Y2, float Width, float Alpha=1.0f);
	void Init();
};