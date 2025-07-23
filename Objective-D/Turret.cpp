#include "Turret.h"
#include "CameraUtil.h"

Turret::Turret(const xmfloat3& createPosition, float createRotation, bool createFromServer) {
	position = createPosition;
	rotation.y = createRotation;
	createdByServer = createFromServer;
}

void Turret::updateBound() {
	xmfloat3 turretPosition = xmfloat3(position.x, position.y + 0.5, position.z);
	frustumBound.Update(turretPosition, xmfloat3(3.0, 3.0, 3.0));
	inFrustum = camera.CheckFrustum(frustumBound);

	lookRange.Update(position, 30.0);
	hitBox.Update(turretPosition, xmfloat3(3.0, 3.0, 3.0), xmfloat3(0.0, rotation.y, 0.0));
}

void Turret::Update(float Delta) {
	updateBound();
	//size_t size = scene.LayerSize(LAYER_MONSTER);
	//for (int i = 0; i < size; i++) {
	//	if (auto monster = scene.ReferLayer(LAYER_MONSTER, i); monster) {

	//	}
	//}
}

void Turret::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, rotation);
	Render3D(MESH.turretTorso, TEX.turret);
	Render3D(MESH.turretBottom, TEX.turret);

	Transform::Rotate(TranslateMatrix, headRotation);
	Render3D(MESH.turretHead, TEX.turret);

	BeginRender();
	SetLightUse(DISABLE_LIGHT);
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, rotation);
	Transform::Rotate(TranslateMatrix, headRotation);
	Transform::Scale(TranslateMatrix, 3.0, 3.0, 3.0);
	Transform::Move(TranslateMatrix, 0.0, 0.3, -0.3);
	Render3D(MESH.gun_flame, TEX.gun_flame);
	Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
}
