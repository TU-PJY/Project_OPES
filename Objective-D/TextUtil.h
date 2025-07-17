#pragma once
#include "DirectX_3D.h"

class Text {
private:
	XMFLOAT4X4 TextMatrix{};
	XMFLOAT3   TextColor{};

	float      TextOpacity{ 1.0 };
	int        TextAlign{ ALIGN_DEFAULT };
	int        TextHeightAlign{ HEIGHT_DEFAULT };


public:
	Text() {}
	Text(int AlignFlag, int HeightFlag, const XMFLOAT3& Color);
	void SetAlign(int Flag);
	void SetHeightAlign(int Flag);
	void SetColor(const XMFLOAT3& Color);
	void SetOpacity(float OpacityValue);
	void Render(const XMFLOAT2& Position, float Size, const std::string& Str);

private:
	void BeginTextRender();
	void PrepareTextRender();
	void RenderText(int Index);
};