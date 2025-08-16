#include "HeavyMachineGun.h"
#include "Bullet.h"
#include "CameraUtil.h"
void SendBangPacket(unsigned int ID);

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

void HeavyMachineGun::ReloadGun() {
	if (currentAmmo == totalAmmo || reloadState)
		return;

	reloadState = true;
	zoomState = false;

	SOUND.machinegunReload.Play();
}

void HeavyMachineGun::updateFire(float Delta) {
	if (currentFireDelayTime > 0.0)
		currentFireDelayTime -= Delta;
	else
		fireEnableState = true;

	if (fireEnableState && triggerState && currentAmmo > 0 && !reloadState) {
		SOUND.machinegunShoot.Play();

		recoilOffset -= recoilBack;
		currentFireDelayTime = fireDelayTime;
		currentFlameRenderTime = flameRenderTime;
		if (userPtr) userPtr->InputRecoil(recoil);
		//{
		//	std::lock_guard<std::mutex> lock(PacketMutex);
			scene.AddObject(new Bullet(damage), "bullet", LAYER3);
			SendBangPacket(GLOBAL.myID);
		//}
		currentAmmo--;
		fireEnableState = false;

		camera.AddRecoilShake(150.0);
	}
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