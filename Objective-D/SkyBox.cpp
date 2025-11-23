#include "SkyBox.h"
#include "CameraUtil.h"

void SkyBox::Render() {
	BeginRender();
	FlipTexture(FLIP_TYPE_NONE);
	SetLightUse(DISABLE_LIGHT);
	SetFogUse(DISABLE_FOG);
	Transform::Move(TranslateMatrix, camera.GetPosition());
	Transform::Scale(ScaleMatrix, XMFLOAT3(600.0, 600.0, 600.0));

	if(GLOBAL.mapName == "map1" || GLOBAL.mapName == "map2")
		Render3D(SYSRES.SkyboxMesh, TEX.skyBox);
	else if(GLOBAL.mapName == "map3")
		Render3D(SYSRES.SkyboxMesh, TEX.skyBox2);
	else
		Render3D(SYSRES.SkyboxMesh, TEX.skyBoxSpace);
}