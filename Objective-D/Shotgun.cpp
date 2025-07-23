#include "Shotgun.h"
#include "Bullet.h"

Shotgun::Shotgun(GameObject* Ptr) {
	// ÅºÈ¯ ÇÏ³ª ´ç ´ë¹ÌÁö (12°³)
	damage = 10;
	recoil = 12.0;
	recoilBack = 0.2;
	totalAmmo = 6;
	currentAmmo = totalAmmo;
	flameRenderTime = 0.05;
	fireDelayTime = 1.0;
	reloadTime = 2.0;

	userPtr = Ptr;
}

void Shotgun::Render() {
	BeginRender();
	xmfloat3 renderPosition = xmfloat3(position.x, position.y - 0.0205, position.z);
	Transform::Move(TranslateMatrix, renderPosition);
	Transform::Rotate(TranslateMatrix, rotation);
	Transform::Move(TranslateMatrix, positionOffset.x + shakeResultX, positionOffset.y + shakeResultY, positionOffset.z + recoilOffset);
	Render3D(MESH.shotgun, TEX.scifi);

	// ·¹µå µµÆ® ·»´õ¸µ
	SetLightUse(DISABLE_LIGHT);
	Render3D(MESH.dot_shotgun, TEX.scifi);

	// È­¿° ·»´õ¸µ
	if (currentFlameRenderTime > 0.0) {
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
	}
}
