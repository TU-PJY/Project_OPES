#include "Bullet.h"
#include "MathUtil.h"
#include "CameraUtil.h"

Bullet::Bullet(const XMFLOAT3& createPosition, const XMFLOAT3& targetPosition, const std::string& terrainName) {
	position = createPosition;
	destination = targetPosition;
	if (auto terrain = scene.Find(terrainName); terrain)
		currentTerrain = terrain;
}

void Bullet::updateTerrainCollision() {
	if (currentTerrain) {
		terrainUtil.InputPosition(position);
		if (terrainUtil.CheckCollision(currentTerrain->GetTerrain()));
	}
}

void Bullet::Update(float Delta) {
	Math::MoveTowardInfinity(position, destination, 30.0, Delta);
}

void Bullet::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Math::LookAt(RotateMatrix, vec, position, camera.GetPosition(), camera.GetUpVector());
	Transform::Scale(ScaleMatrix, XMFLOAT3(0.2, 0.2, 0.2));
	SetColor(XMFLOAT3(1.0, 1.0, 1.0));
	Render3D(SYSRES.BillboardMesh, TEX.ColorTex);
}
