#include "SkyBox.h"
#include "CameraUtil.h"

void SkyBox::Render() {
	BeginRender();
	FlipTexture(FLIP_TYPE_NONE);
	SetLightUse(DISABLE_LIGHT);
	Transform::Move(TranslateMatrix, camera.GetPosition());
	Transform::Scale(ScaleMatrix, XMFLOAT3(300.0, 300.0, 300.0));
	Render3D(SYSRES.SkyboxMesh, TEX.skyBox);
}