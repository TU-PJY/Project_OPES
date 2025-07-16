#include "SkyBox.h"

void SkyBox::Render() {
	BeginRender();
	FlipTexture(FLIP_TYPE_NONE);
	SetLightUse(DISABLE_LIGHT);
	Transform::Scale(ScaleMatrix, XMFLOAT3(300.0, 300.0, 300.0));
	Render3D(SysRes.SkyboxMesh, TEX.skyBox);
}