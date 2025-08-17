#include "Shotgun.h"
#include "SpreadBullet.h"
#include "CameraUtil.h"
#include "ClampUtil.h"

void SendBangPacket(unsigned int ID);

Shotgun::Shotgun(GameObject* Ptr) {
	// 탄환 하나 당 대미지 (12개)
	damage = SG_DAMAGE;
	recoil = SG_RECOIL;
	recoilBack = SG_RECOIL_BACK;
	totalAmmo = SG_MAGAZINE;
	currentAmmo = totalAmmo;
	flameRenderTime = FLAME_RENDER_TIME;
	fireDelayTime = SG_SHOOT_DELAY;
	reloadTime = SG_RELOAD_TIME;

	userPtr = Ptr;

	// SHOOT_SPEED_INCREASE 활성화 시 연사 속도 20% 증가
	if (GLOBAL.buff[SHOOT_SPEED_INCREASE])
		fireDelayTime *= 0.6;

	// RELOAD_SPEED_INCREASE 활성화 시 재장전 시간 50% 감소
	if (GLOBAL.buff[RELOAD_SPEED_INCREASE])
		reloadTime *= 0.5;
}

void Shotgun::Render() {
	BeginRender();
	xmfloat3 renderPosition = xmfloat3(position.x, position.y - 0.0205, position.z);
	Transform::Move(TranslateMatrix, renderPosition);
	Transform::Rotate(TranslateMatrix, rotation);
	Transform::Move(TranslateMatrix, positionOffset.x + shakeResultX, positionOffset.y + shakeResultY, positionOffset.z + recoilOffset);
	Render3D(MESH.shotgun, TEX.scifi);

	// 레드 도트 렌더링
	SetLightUse(DISABLE_LIGHT);
	Render3D(MESH.dot_shotgun, TEX.scifi);

	// 화염 렌더링
	if (currentFlameRenderTime > 0.0) {
		Transform::Scale(ScaleMatrix, 1.5, 1.5, 1.0);
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
			camera.AddRecoilShake(200.0, 0.7, 40.0);
		//}
		currentAmmo--;
		fireEnableState = false;
	}
}

void Shotgun::updateGun(float Delta) {
	// 총 회전 업데이트
	rotation.x = std::lerp(rotation.x, rotationDest.x, Delta * 30.0);
	rotation.y = std::lerp(rotation.y, rotationDest.y, Delta * 30.0);
	rotation.z = std::lerp(rotation.z, rotationDest.z, Delta * 30.0);

	// 줌에 따른 위치 오프셋 업데이트
	if (!reloadState) {
		if (zoomState) {
			positionOffset.x = std::lerp(positionOffset.x, 0.0, Delta * 20.0);
			positionOffset.y = std::lerp(positionOffset.y, -0.22, Delta * 20.0);
			positionOffset.z = std::lerp(positionOffset.z, 0.2, Delta * 20.0);
		}
		else {
			positionOffset.x = std::lerp(positionOffset.x, 0.3, Delta * 20.0);
			positionOffset.y = std::lerp(positionOffset.y, -0.3, Delta * 20.0);
			positionOffset.z = std::lerp(positionOffset.z, 0.3, Delta * 20.0);
		}
	}
	else {
		positionOffset.x = std::lerp(positionOffset.x, 0.3, Delta * 10.0);
		positionOffset.y = std::lerp(positionOffset.y, -0.6, Delta * 10.0);
		positionOffset.z = std::lerp(positionOffset.z, 0.3, Delta * 10.0);
	}

	// 반동에 따른 위치 오프셋 업데이트
	recoilOffset = std::lerp(recoilOffset, 0.0, Delta * 4.0);

	// 화염 렌더링시간 업데이트
	currentFlameRenderTime -= Delta;
	Clamp::LimitValue(currentFlameRenderTime, 0.0, CLAMP_DIR_LESS);
}
