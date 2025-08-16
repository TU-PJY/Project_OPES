#include "Shotgun.h"
#include "SpreadBullet.h"
#include "CameraUtil.h"
void SendBangPacket(unsigned int ID);


Shotgun::Shotgun(GameObject* Ptr) {
	// ÅºÈ¯ ÇÏ³ª ´ç ´ë¹ÌÁö (12°³)
	damage = SG_DAMAGE;
	recoil = SG_RECOIL;
	recoilBack = SG_RECOIL_BACK;
	totalAmmo = SG_MAGAZINE;
	currentAmmo = totalAmmo;
	flameRenderTime = FLAME_RENDER_TIME;
	fireDelayTime = SG_SHOOT_DELAY;
	reloadTime = SG_RELOAD_TIME;

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

void Shotgun::ReloadGun() {
	if (currentAmmo == totalAmmo || reloadState)
		return;

	reloadState = true;
	zoomState = false;

	SOUND.machinegunReload.Play();
}

void Shotgun::updateFire(float Delta) {
	if (currentFireDelayTime > 0.0)
		currentFireDelayTime -= Delta;
	else
		fireEnableState = true;

	if (fireEnableState && triggerState && currentAmmo > 0 && !reloadState) {
		SOUND.shotgunShoot.Play();

		recoilOffset -= recoilBack;
		currentFireDelayTime = fireDelayTime;
		currentFlameRenderTime = flameRenderTime;
		if (userPtr) userPtr->InputRecoil(recoil);
		/*{
			std::lock_guard<std::mutex> lock(PacketMutex);*/
			scene.AddObject(new SpreadBullet(SG_DAMAGE), "spreadBullet", LAYER3);
			SendBangPacket(GLOBAL.myID);
			camera.AddRecoilShake(300.0, 0.5);
		//}
		currentAmmo--;
		fireEnableState = false;
	}
}
