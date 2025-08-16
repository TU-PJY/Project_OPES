#include "DMR.h"
#include "ClampUtil.h"
#include "Bullet.h"
#include "CameraUtil.h"

DMR::DMR(GameObject* Ptr) {
	damage = DMR_DAMAGE;
	recoil = DMR_RECOIL;
	recoilBack = DMR_RECOIL_BACK;
	totalAmmo = DMR_MAGAZINE;
	currentAmmo = totalAmmo;
	flameRenderTime = FLAME_RENDER_TIME;
	fireDelayTime = DMR_SHOOT_DELAY;
	reloadTime = DMR_RELOAD_TIME;

	userPtr = Ptr;
}

void DMR::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, rotation);
	Transform::Move(TranslateMatrix, positionOffset.x + shakeResultX, positionOffset.y + shakeResultY, positionOffset.z + recoilOffset);
	Render3D(MESH.dmr, TEX.scifi);
}

void DMR::ReloadGun() {
	if (currentAmmo == totalAmmo || reloadState)
		return;

	reloadState = true;
	zoomState = false;

	SOUND.dmrReload.Play();
}

void DMR::updateFire(float Delta) {
	if (currentFireDelayTime > 0.0)
		currentFireDelayTime -= Delta;
	else
		fireEnableState = true;

	if (fireEnableState && triggerState && currentAmmo > 0 && !reloadState) {
		SOUND.dmrShoot.Play();

		recoilOffset -= recoilBack;
		currentFireDelayTime = fireDelayTime;
		currentFlameRenderTime = flameRenderTime;
		//if (userPtr) userPtr->InputRecoil(recoil);
		//{
		//std::lock_guard<std::mutex> lock(PacketMutex);
		scene.AddObject(new Bullet(damage), "bullet", LAYER3);
		camera.AddRecoilShake(200.0);
	//	}
		currentAmmo--;
		fireEnableState = false;

		if (auto scope = scene.Find("scope"); scope)
			scope->SetSize(8.0);
	}
}

void DMR::updateGun(float Delta) {
	// 총 회전 업데이트
	rotation.x = std::lerp(rotation.x, rotationDest.x, Delta * 30.0);
	rotation.y = std::lerp(rotation.y, rotationDest.y, Delta * 30.0);
	rotation.z = std::lerp(rotation.z, rotationDest.z, Delta * 30.0);

	if(reloadState) {
		positionOffset.x = std::lerp(positionOffset.x, 0.3, Delta * 10.0);
		positionOffset.y = std::lerp(positionOffset.y, -0.6, Delta * 10.0);
		positionOffset.z = std::lerp(positionOffset.z, 0.3, Delta * 10.0);
	}

	else {
		positionOffset.x = std::lerp(positionOffset.x, 0.3, Delta * 20.0);
		positionOffset.y = std::lerp(positionOffset.y, -0.3, Delta * 20.0);
		positionOffset.z = std::lerp(positionOffset.z, 0.3, Delta * 20.0);
	}

	// 반동에 따른 위치 오프셋 업데이트
	recoilOffset = std::lerp(recoilOffset, 0.0, Delta * 10.0);

	// 화염 렌더링시간 업데이트
	currentFlameRenderTime -= Delta;
	Clamp::LimitValue(currentFlameRenderTime, 0.0, CLAMP_DIR_LESS);
}
