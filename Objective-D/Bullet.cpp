#include "Bullet.h"
#include "MathUtil.h"
#include "CameraUtil.h"
#include "ClampUtil.h"

Bullet::Bullet(const XMFLOAT3& createPosition, float degreesY, float degreesX, int damage, const std::string& terrainName) {
	position = createPosition;
	startPosition = position;
	currentMapName = terrainName;

	if (auto terrain = scene.Find(terrainName); terrain) {
		currentTerrain = terrain;
		mapOOBBData = terrain->GetMapWallOOBB();
	}

	this->degreesY = degreesY;
	this->degreesX = degreesX;
	this->bulletDamage = damage;
}

void Bullet::updateTerrainCollision() {
	if (currentTerrain) {
		terrainUtil.InputPosition(position);
		if (terrainUtil.CheckCollision(currentTerrain->GetTerrain()))
			scene.DeleteObject(this);
	}
}

void Bullet::updateCollision() {
	if (currentMapName.compare("map1") == 0) {
		size_t size = scene.LayerSize(LAYER2);
		for (int i = 0; i < size; i++) {
			if (auto enemy = scene.FindMulti("plantMonster", LAYER2, i); enemy) {
				if (enemy->CheckHit(bulletBound, bulletDamage)) {
					scene.DeleteObject(this);
					return;
				}
			}
		}
	}

	for (auto& O : mapOOBBData) {
		if (bulletBound.CheckCollision(O)) {
			scene.DeleteObject(this);
			return;
		}
	}
}

void Bullet::updateBound() {
	bulletBound.Update(position, 0.2);
}

void Bullet::Update(float Delta) {
	bulletOpacity += Delta * 15.0;
	Clamp::LimitValue(bulletOpacity, 1.0, CLAMP_DIR_GREATER);

	Math::MoveInDirection(position, degreesY, degreesX, 250.0, Delta);

	if (Math::CalcDistance3D(position, startPosition) >= 300.0) {
		scene.DeleteObject(this);
		return;
	}

	updateBound();
	updateCollision();
	updateTerrainCollision();
}

void Bullet::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Math::LookAt(RotateMatrix, vec, position, camera.GetPosition(), camera.GetUpVector());
	Transform::Scale(ScaleMatrix, XMFLOAT3(0.2, 0.2, 0.2));
	SetColor(XMFLOAT3(1.0, 1.0, 0.0));
	Render3D(SYSRES.BillboardMesh, TEX.ColorTex, bulletOpacity);

	bulletBound.Render();
}
