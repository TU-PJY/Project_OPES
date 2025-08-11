#pragma once
#include "DirectX_3D.h"

class Text {
private:
	XMFLOAT4X4 TextMatrix{};
	XMFLOAT3   TextColor{};

	float      TextOpacity{ 1.0 };
	int        TextAlign{ ALIGN_DEFAULT };
	int        TextHeightAlign{ HEIGHT_DEFAULT };

	bool       TextShadowState{};
	XMFLOAT2   TextShadowOffset{};
	float      TextShadowOpacity{ 0.3 };

	XMFLOAT3   TextRenderColor{};
	float      TextRenderOpacity{};

	bool       TextStaticSize{};

public:
	Text() {}
	Text(int AlignFlag, int HeightFlag, const XMFLOAT3& Color);
	void SetShadow(const XMFLOAT2& ShadowOffset, float ShadowOpacity);
	void EnableShadow();
	void DisableShadow();
	void SetAlign(int Flag);
	void SetHeightAlign(int Flag);
	void SetColor(const XMFLOAT3& Color);
	void SetOpacity(float OpacityValue);
	void EnableStaticSize();
	void DisableStaticSize();
	void Render3D(const XMFLOAT3& Position, float Size, const std::string& Str);
	void Render(const XMFLOAT2& Position, float Size, const std::string& Str);


private:
	void TransformText3D(const XMFLOAT3& Position, float Size, float TotalLength, int StrLength, const char* Input);
	void TransformText(const XMFLOAT2& Position, float Size, float TotalLength, int StrLength, const char* Input);
	void BeginTextRender();
	void PrepareTextRender();
	void RenderText(int Index);
	void RenderText3D(int Index);
	void BeginTextRender3D();
	void PrepareTextRender3D();
};