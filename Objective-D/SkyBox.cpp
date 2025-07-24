#include "SkyBox.h"
#include "CameraUtil.h"

void SkyBox::Render() {
	BeginRender();
	FlipTexture(FLIP_TYPE_NONE);
	SetLightUse(DISABLE_LIGHT);
	Transform::Move(TranslateMatrix, camera.GetPosition());
	Transform::Scale(ScaleMatrix, XMFLOAT3(600.0, 600.0, 600.0));

	if(GLOBAL.mapName.compare("map3") != 0)
		Render3D(SYSRES.SkyboxMesh, TEX.skyBox);
	else
		Render3D(SYSRES.SkyboxMesh, TEX.skyBox2);
}