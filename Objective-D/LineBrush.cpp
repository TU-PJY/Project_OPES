#include "LineBrush.h"
#include "TextureUtil.h"
#include "ShaderUtil.h"
#include "MeshUtil.h"
#include "GameResource.h"
#include "CBVUtil.h"
#include "TransformUtil.h"
#include "CameraUtil.h"
#include "RootConstants.h"
#include "MathUtil.h"
#include "RootConstantUtil.h"

// 선의 색상을 지정한다.
void LineBrush::SetColor(float R, float G, float B) {
	LineColor.x = R;
	LineColor.y = G;
	LineColor.z = B;
}

void LineBrush::Init() {
	Transform::Identity(TranslateMatrix);
	Transform::Identity(ScaleMatrix);
	TransparencyValue = 1.0f;

	camera.SetToStaticMode();
	camera.SetViewMatrix();
	camera.GenerateStaticMatrix();
	camera.SetViewportsAndScissorRects();
	camera.UpdateShaderVariables();
}

// 선을 그린다.
void LineBrush::Draw(float X1, float Y1, float X2, float Y2, float Width, float Alpha) {
	Init();

	TransparencyValue = Alpha;

	Length = Math::CalcDistance2D(X1, Y1, X2, Y2);
	Rotation = Math::CalcDegree2D(X1, Y1, X2, Y2);

	Transform::Move(TranslateMatrix, X1, Y1, 0.0);
	Transform::Rotate(TranslateMatrix, 0.0, 0.0, Rotation);
	Transform::Move(TranslateMatrix, Length / 2.0, 0.0, 0.0);
	Transform::Scale(ScaleMatrix, Width + Length, Width, 1.0);

	LineShader->RenderDepthNone(ObjectCmdList);

	XMMATRIX ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), XMLoadFloat4x4(&TranslateMatrix));
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(ObjectCmdList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(ObjectCmdList, &LineColor, GAME_OBJECT_INDEX, 3, 16);
	RCUtil::Input(ObjectCmdList, &TransparencyValue, GAME_OBJECT_INDEX, 1, 19);

	ImagePannel->Render(ObjectCmdList);
}