#include "HeavyMachineGun.h"

HeavyMachineGun::HeavyMachineGun(GameObject* Ptr) {
	damage = 10;
	recoil = 2.5;
	totalAmmo = 80;
	currentAmmo = totalAmmo;
	flameRenderTime = 0.03;
	fireDelayTime = 0.1;
	reloadTime = 3.0;

	userPtr = Ptr;
}

void HeavyMachineGun::Render() {
	// ÃÑ ·»´õ¸µ
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, rotation);
	Transform::Move(TranslateMatrix, positionOffset.x + shakeResultX, positionOffset.y + shakeResultY, positionOffset.z + recoilOffset);
	Render3D(MESH.machine_gun, TEX.scifi);

	// ·¹µå µµÆ® ·»´õ¸µ
	SetLightUse(DISABLE_LIGHT);
	Render3D(MESH.dot_machine_gun, TEX.scifi);

	// È­¿° ·»´õ¸µ
	if (currentFlameRenderTime > 0.0) {
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
	}
}
