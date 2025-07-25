#include "HeavyMachineGun.h"
#include "ClampUtil.h"
#include "Bullet.h"

void Gun::enableZoom() {
	if (reloadState)
		return;

	zoomState = true;
}

void Gun::disableZoom() {
	zoomState = false;
}

void Gun::pullTrigger() {
	triggerState = true;
}

void Gun::releaseTrigger() {
	triggerState = false;
}

void Gun::InputPosition(XMFLOAT3& position) {
	this->position = position;
}

void Gun::inputRotation(const XMFLOAT3& rotation) {
	rotationDest = rotation;
}

void Gun::inputMoveState(bool moveState) {
	this->moveState = moveState;
}

int Gun::getTotalAmmo() {
	return totalAmmo;
}

int Gun::getCurrentAmmo() {
	return currentAmmo;
}

bool Gun::getZoomState() {
	return zoomState;
}

bool Gun::getReloadState() {
	return reloadState;
}

void Gun::ReloadGun() {
	if (currentAmmo == totalAmmo || reloadState)
		return;

	reloadState = true;
	zoomState = false;
}

// 총의 위치, 회전을 업데이트 한다.
void Gun::updateGun(float Delta) {
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
	recoilOffset = std::lerp(recoilOffset, 0.0, Delta * 10.0);

	// 화염 렌더링시간 업데이트
	currentFlameRenderTime -= Delta;
	Clamp::LimitValue(currentFlameRenderTime, 0.0, CLAMP_DIR_LESS);
}

// 총알 발사 업데이트
void Gun::updateFire(float Delta) {
	if (currentFireDelayTime > 0.0)
		currentFireDelayTime -= Delta;
	else
		fireEnableState = true;

	if (fireEnableState && triggerState && currentAmmo > 0 && !reloadState) {
		recoilOffset -= recoilBack;
		currentFireDelayTime = fireDelayTime;
		currentFlameRenderTime = flameRenderTime;
		if (userPtr) userPtr->InputRecoil(recoil);
		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			scene.AddObject(new Bullet(damage), "bullet", LAYER3);
		}
		currentAmmo--;
		fireEnableState = false;
	}
}

void Gun::updateAnimation(float Delta) {
	if (moveState)
		shakeRatio = std::lerp(shakeRatio, 0.01, 10.0 * Delta);
	else
		shakeRatio = std::lerp(shakeRatio, 0.0, 10.0 * Delta);

	shakeValueX += Delta * 8.0;
	shakeValueY += Delta * 16.0;
	shakeResultX = sinf(shakeValueX) * shakeRatio;
	shakeResultY = sinf(shakeValueY) * shakeRatio;
}

void Gun::updateReload(float Delta) {
	if (!reloadState)
		return;

	reloadDelayTime += Delta;
	if (reloadDelayTime >= reloadTime) {
		reloadDelayTime = 0.0;
		currentAmmo = totalAmmo;
		reloadState = false;
	}
}

void Gun::Update(float Delta) {
	updateGun(Delta);
	updateFire(Delta);
	updateAnimation(Delta);
	updateReload(Delta);
}
