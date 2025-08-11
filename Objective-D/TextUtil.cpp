#include "TextUtil.h"
#include "GameResource.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"
#include "CBVUtil.h"
#include "TransformUtil.h"
#include "CameraUtil.h"
#include "MathUtil.h"

Text::Text(int AlignFlag, int HeightFlag, const XMFLOAT3& Color) {
	SetAlign(AlignFlag);
	SetHeightAlign(HeightFlag);
	SetColor(Color);
}

void Text::SetShadow(const XMFLOAT2& ShadowOffset, float ShadowOpacity) {
	TextShadowOffset = ShadowOffset;
	TextShadowOpacity = ShadowOpacity;
}

void Text::EnableShadow() {
	TextShadowState = true;
}

void Text::DisableShadow() {
	TextShadowState = false;
}

void Text::SetAlign(int Flag) {
	TextAlign = Flag;
}

void Text::SetHeightAlign(int Flag) {
	TextHeightAlign = Flag;
}

void Text::SetColor(const XMFLOAT3& Color) {
	TextColor = Color;
}

void Text::SetOpacity(float OpacityValue) {
	TextOpacity = OpacityValue;
}

void Text::EnableStaticSize() {
	TextStaticSize = true;
}

void Text::DisableStaticSize() {
	TextStaticSize = false;
}

void Text::Render(const XMFLOAT2& Position, float Size, const std::string& Str) {
	const char* Input = Str.c_str();
	int Length = Str.length();
	float TotalLength = (float)(Length - 1) * Size * 0.45;

	XMFLOAT2 RenderStartPosition{};

	if (TextShadowState) {
		BeginTextRender();
		TextRenderOpacity = TextOpacity * TextShadowOpacity;
		TextRenderColor = XMFLOAT3(0.0, 0.0, 0.0);
		RenderStartPosition = XMFLOAT2(Position.x + TextShadowOffset.x, Position.y + TextShadowOffset.y);
		TransformText(RenderStartPosition, Size, TotalLength, Length, Input);
	}

	BeginTextRender();
	TextRenderOpacity = TextOpacity;
	TextRenderColor = TextColor;
	RenderStartPosition = Position;
	TransformText(RenderStartPosition, Size, TotalLength, Length, Input);
}


void Text::TransformText(const XMFLOAT2& Position, float Size, float TotalLength, int StrLength, const char* Input) {
	switch (TextAlign) {
	case ALIGN_DEFAULT:
		Transform::Move2D(TextMatrix, Position.x, Position.y); break;
	case ALIGN_MIDDLE:
		Transform::Move2D(TextMatrix, Position.x - TotalLength * 0.5, Position.y); break;
	case ALIGN_LEFT:
		Transform::Move2D(TextMatrix, Position.x - TotalLength, Position.y); break;
	}

	switch (TextHeightAlign) {
	case HEIGHT_DEFAULT:
		Transform::Move2D(TextMatrix, 0.0, Size * 0.5); break;
	case HEIGHT_MIDDLE:
		break;
	case HEIGHT_UNDER:
		Transform::Move2D(TextMatrix, 0.0, -Size * 0.5); break;
	}

	Transform::Scale2D(TextMatrix, Size, Size);

	for (int i = 0; i < StrLength; i++) {
		if (i > 0)
			Transform::Move2D(TextMatrix, 0.45, 0.0);
		char Word = Input[i];
		int Index = static_cast<int>(Input[i]);
		if (0 < Index - 32 && Index - 32 < 96)
			RenderText(Index - 32);
	}
}

void Text::BeginTextRender() {
	Transform::Identity(TextMatrix);
	GlobalCommandList->SetGraphicsRootSignature(ImageShaderRootSignature);
	CBVUtil::Input(GlobalCommandList, FlipCBV, FLIP_TYPE_V);
	camera.SetToStaticMode();
}

void Text::PrepareTextRender() {
	camera.GenerateStaticMatrix();
	camera.UpdateShaderVariables();

	XMFLOAT4X4 TextResultMatrix;
	XMStoreFloat4x4(&TextResultMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TextMatrix)));

	RCUtil::Input(GlobalCommandList, &TextResultMatrix, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &TextRenderColor, GAME_OBJECT_INDEX, 3, 16);
	RCUtil::Input(GlobalCommandList, &TextRenderOpacity, GAME_OBJECT_INDEX, 1, 19);
}

void Text::RenderText(int Index) {
	TEX.fontAtlas[Index]->Render2D(GlobalCommandList);
	ImageShader->RenderDepthNone(GlobalCommandList);
	PrepareTextRender();
	SYSRES.ImagePannel->Render(GlobalCommandList);
}



void Text::Render3D(const XMFLOAT3& Position, float Size, const std::string& Str) {
	float InputSize = Size;
	if (TextStaticSize) {
		float Distance = Math::CalcDistance3D(Position, camera.GetPosition());
		InputSize *= Distance;
	}

	const char* Input = Str.c_str();
	int Length = Str.length();
	float TotalLength = (float)(Length - 1) * InputSize * 0.45;

	XMFLOAT3 RenderStartPosition{};

	if (TextShadowState) {
		BeginTextRender3D();
		TextRenderOpacity = TextOpacity * TextShadowOpacity;
		TextRenderColor = XMFLOAT3(0.0, 0.0, 0.0);
		RenderStartPosition = XMFLOAT3(Position.x + TextShadowOffset.x, Position.y + TextShadowOffset.y, Position.z);
		TransformText3D(RenderStartPosition, InputSize, TotalLength, Length, Input);
	}

	BeginTextRender3D();
	TextRenderOpacity = TextOpacity;
	TextRenderColor = TextColor;
	RenderStartPosition = Position;
	TransformText3D(RenderStartPosition, InputSize, TotalLength, Length, Input);
}

void Text::TransformText3D(const XMFLOAT3& Position, float Size, float TotalLength, int StrLength, const char* Input) {
	switch (TextAlign) {
	case ALIGN_DEFAULT:
		Transform::Move(TextMatrix, Position.x, Position.y, Position.z); break;
	case ALIGN_MIDDLE:
		Transform::Move(TextMatrix, Position.x - TotalLength * 0.5, Position.y, Position.z); break;
	case ALIGN_LEFT:
		Transform::Move(TextMatrix, Position.x - TotalLength, Position.y, Position.z); break;
	}

	Vector vec{};
	Math::BillboardLookAt(TextMatrix, vec, (XMFLOAT3)Position, camera.GetPosition());

	switch (TextHeightAlign) {
	case HEIGHT_DEFAULT:
		Transform::Move2D(TextMatrix, 0.0, Size * 0.5); break;
	case HEIGHT_MIDDLE:
		break;
	case HEIGHT_UNDER:
		Transform::Move2D(TextMatrix, 0.0, -Size * 0.5); break;
	}

	Transform::Scale2D(TextMatrix, Size, Size);

	for (int i = 0; i < StrLength; i++) {
		if (i > 0)
			Transform::Move2D(TextMatrix, 0.45, 0.0);
		char Word = Input[i];
		int Index = static_cast<int>(Input[i]);
		if (0 < Index - 32 && Index - 32 < 96)
			RenderText3D(Index - 32);
	}
}

void Text::BeginTextRender3D() {
	Transform::Identity(TextMatrix);
	GlobalCommandList->SetGraphicsRootSignature(ObjectShaderRootSignature);
	CBVUtil::Input(GlobalCommandList, FlipCBV, FLIP_TYPE_V);
	camera.SetToDefaultMode();
}

void Text::PrepareTextRender3D() {
	camera.GeneratePerspectiveMatrix(0.1f, 1300.0f, ASPECT, 55.0f + GLOBAL.offsetFOV);
	camera.UpdateShaderVariables();

	XMFLOAT4X4 TextResultMatrix;
	XMStoreFloat4x4(&TextResultMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TextMatrix)));

	RCUtil::Input(GlobalCommandList, &TextResultMatrix, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &TextRenderColor, GAME_OBJECT_INDEX, 3, 16);
	RCUtil::Input(GlobalCommandList, &TextRenderOpacity, GAME_OBJECT_INDEX, 1, 19);
}

void Text::RenderText3D(int Index) {
	TEX.fontAtlas[Index]->Render3D(GlobalCommandList);
	ObjectShader->RenderDepthNone(GlobalCommandList);
	PrepareTextRender3D();
	SYSRES.ImagePannel->Render(GlobalCommandList);
}