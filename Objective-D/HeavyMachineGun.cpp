#include "HeavyMachineGun.h"


HeavyMachineGun::HeavyMachineGun(GameObject* Ptr) {
	damage = MG_DAMAGE;
	recoil = MG_RECOIL;
	recoilBack = MG_RECOIL_BACK;
	totalAmmo = MG_MAGAZINE;
	currentAmmo = totalAmmo;
	flameRenderTime = FLAME_RENDER_TIME;
	fireDelayTime = MG_SHOOT_DELAY;
	reloadTime = MG_RELOAD_TIME;

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