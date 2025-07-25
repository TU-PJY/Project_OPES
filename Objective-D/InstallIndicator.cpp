#include "InstallIndicator.h"
#include "CameraUtil.h"

void InstallIndicator::SetItem(int num) {
	currentItem = num;
}

void InstallIndicator::SetRenderState(bool flag) {
	renderState = flag;
}

void InstallIndicator::Render() {
	if (!renderState)
		return;

	float distance;
	position = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, distance);
	rotation.y = XMConvertToDegrees(camera.GetYaw());

	if (currentItem == 0) {
		BeginRender();
		if (distance >= 8.0)
			SetColor(1.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(TranslateMatrix, rotation);
		Render3D(MESH.turretTorso, TEX.turret, 0.4);
		Render3D(MESH.turretBottom, TEX.turret, 0.4);
		Render3D(MESH.turretHead, TEX.turret, 0.4);
	}

	else if (currentItem == 1) {
		BeginRender();
		if (distance >= 8.0)
			SetColor(1.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(RotateMatrix, rotation);
		Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
		Render3D(MESH.beacon, TEX.scifi, 0.4);
	}
}
