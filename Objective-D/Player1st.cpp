#include "Player1st.h"
#include "PlayerHit.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ClampUtil.h"
#include "HeavyMachineGun.h"
#include "PlayerIndicator.h"

#include "Grenade.h"

void SendMovePacket(float x, float y, float z);
void SendViewingAnglePacket(float x, float y, float z);
void SendAnimaionPacket(unsigned short playerState);

Player1st::Player1st(int characterType) {
	// 현재 캐릭터 타입에 맞는 무기 객체를 추가 후 연결한다.
	switch (characterType) {
	case CHARACTER_MG:
		weaponPtr = scene.AddObject(new HeavyMachineGun(this), "mg", LAYER4);
		maxSpeed = 8.0;
		totalHP = 100;
		currentHP = 100;
		break;
	}

	currentSpeed = maxSpeed;

	// 플레이어 인디케이터에 필요한 정보들을 전달한다.
	IndicatorPtr = scene.AddObject(new PlayerIndicator, "playerInd", LAYERUI);
	if (IndicatorPtr) {
		IndicatorPtr->InputHP(totalHP, currentHP);
		IndicatorPtr->InputGrenade(currentGrenadeCount);
		if (weaponPtr) {
			int totalAmmo = weaponPtr->getTotalAmmo();
			int currentAmmo = weaponPtr->getCurrentAmmo();
			IndicatorPtr->InputAmmo(totalAmmo, currentAmmo);
		}
	}
}

Player1st::~Player1st() {
	if (weaponPtr)
		scene.DeleteObject(weaponPtr);
	if (IndicatorPtr)
		scene.DeleteObject(IndicatorPtr);
}

void Player1st::sendPacket(float Delta) {
	currentPacketSendDelay += Delta;
	if (currentPacketSendDelay >= packetSendDelay) {
		currentPacketSendDelay -= packetSendDelay;

		switch (sendOrder) {
		case 1:
			if (currentState == STATE_MOVE || currentState == STATE_MOVE_SHOOT)
				SendMovePacket(playerPosition.x, playerPosition.y, playerPosition.z);
			break;

		case 2:
			if (prevRotation.y != currentRotation.y) {
				SendViewingAnglePacket(currentRotation.x, currentRotation.y, currentRotation.z);
				prevRotation = currentRotation;
			}
			break;

		case 3:
			SendAnimaionPacket(currentState);
			break;
		}

		sendOrder += 1;
		if (sendOrder > 3)
			sendOrder = 1;
	}
}

void Player1st::InputMouseMotion(MotionEvent& Event) {
	if (GetCapture() == Event.CaptureState) {
		mouse.HideCursor();
		GetCapture();

		// 정조준 시 감도를 절반으로 낮춘다
		float sensivity = 0.08;
		if (zoomState)  
			sensivity = 0.04;
		XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, sensivity);
		UpdateMotionRotation(currentRotation, Delta.x, Delta.y);

		// 플레이어 시야 회전 제한 
		// 위, 아래 회전을 180 범위로 제한한다.
		Clamp::ClampValue(currentRotation.x, -90.0, 90.0, CLAMP_FIX);
		Clamp::ClampValue(currentRotation.z, -90.0, 90.0, CLAMP_FIX);
	}
}

void Player1st::InputMouse(MouseEvent& Event) {
	// 총 발사 상태 활성화 / 비활성화
	switch (Event.Type) {
	case WM_LBUTTONDOWN:
		// 마우스 모션 캡쳐 상태가 해제된 경우(윈도우 버튼 등으로 다른 윈도우에 포커싱된 경우)
		// 원래의 윈도우에 좌클릭으로 포커싱하면 모션 캡쳐 상태가 다시 활성화 된다.
		mouse.StartMotionCapture(GlobalHWND);
		if (weaponPtr) weaponPtr->pullTrigger();
		triggerState = true;
		break;

	case WM_LBUTTONUP:
		if (weaponPtr) weaponPtr->releaseTrigger();
		triggerState = false;
		break;

	case WM_RBUTTONDOWN:
		if (weaponPtr && !weaponPtr->getReloadState()) {
			weaponPtr->enableZoom();
			zoomState = true;
			destFOV = -20.0;
			currentSpeed = maxSpeed * 0.5;
		}
		break;

	case WM_RBUTTONUP:
		if (weaponPtr) weaponPtr->disableZoom();
		zoomState = false;
		destFOV = 0.0;
		currentSpeed = maxSpeed;
		break;

	case WM_MBUTTONDOWN:
	{
		//최대 2개까지 던지기 가능하다.
		if (currentGrenadeCount > 0) {
			XMFLOAT3 rotation = XMFLOAT3(-currentRotation.x, currentRotation.y, currentRotation.z);
			XMFLOAT3 createPosition = cameraPosition;
			Math::CalcForwardOffset(createPosition, currentRotation.y, 2.0, 0.0);
			scene.AddObject(new Grenade(createPosition, rotation), "grenade", LAYER3);
			currentGrenadeCount--;
			if(IndicatorPtr)
				IndicatorPtr->InputGrenade(currentGrenadeCount);
		}
	}
		break;
	}
}

// 4방향 이동 토글
void Player1st::InputKey(KeyEvent& Event) {
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'W', moveState[FRONT]);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'S', moveState[BACK]);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'A', moveState[LEFT]);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'D', moveState[RIGHT]);

	if (Event.Type == WM_KEYDOWN) {
		switch (Event.Key) {
		case 'R':
			if (weaponPtr) {
				weaponPtr->ReloadGun();
				if (weaponPtr->getReloadState()) {
					zoomState = false;
					destFOV = 0.0;
				}
			}
			break;
		}
	}
}

// 상태를 업데이트 한다.
void Player1st::updateState() {
	if (currentState == STATE_DEATH) return;

	if ((moveState[FRONT] && !moveState[BACK]) || (!moveState[FRONT] && moveState[BACK]) ||
		(moveState[RIGHT] && !moveState[LEFT]) || (!moveState[RIGHT] && moveState[LEFT])) {
		if (triggerState)
			currentState = STATE_MOVE_SHOOT;
		else
			currentState = STATE_MOVE;
	}

	else {
		if (triggerState)
			currentState = STATE_IDLE_SHOOT;
		else
			currentState = STATE_IDLE;
	}
}

// 플레이어 이동 속도를 업데이트한다.
void Player1st::updateMove(float Delta) {
	if (currentState == STATE_DEATH) return;

	// 앞뒤 가속/감속
	if (moveState[FRONT] && !moveState[BACK])
		forwardSpeed = std::lerp(forwardSpeed, currentSpeed, 10.0 * Delta);
	else if (!moveState[FRONT] && moveState[BACK])
		forwardSpeed = std::lerp(forwardSpeed, -currentSpeed, 10.0 * Delta);
	else 
		forwardSpeed = std::lerp(forwardSpeed, 0.0, 10.0 * Delta);

	// 좌우 가속/감속
	if (moveState[RIGHT] && !moveState[LEFT])
		strafeSpeed = std::lerp(strafeSpeed, currentSpeed, 10.0 * Delta);
	else if (!moveState[RIGHT] && moveState[LEFT])
		strafeSpeed = std::lerp(strafeSpeed, -currentSpeed, 10.0 * Delta);
	else
		strafeSpeed = std::lerp(strafeSpeed, 0.0, 10.0 * Delta);

	// 맵 바운드와 충돌을 체크하면서 이동
	Math::MoveWithSlide(playerPosition, currentRotation.y, forwardSpeed, strafeSpeed, playerSphere, GLOBAL.mapOOBBdata, Delta);

	// 카메라 위치를 플레이어 위치와 동기화 
	cameraPosition = playerPosition;
}

// 플레이어 위치 - 터레인 충돌 처리를 업데이트 한다.
void Player1st::updateTerrainCollision() {
	terrainUtil.InputPosition(playerPosition);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, playerPosition, 0.0);

	// 카메라의 경우 플레이어 모델의 실제 눈 높이에 위치하도록 한다.
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, cameraPosition, playerSize.y * 1.5);
}

// 카메라를 업데이트 한다.
void Player1st::updateCamera(float Delta) {
	if (currentState == STATE_DEATH) return;

	// 벡터 및 카메라 추적 업데이트
	Math::UpdateVector(playerVector, currentRotation);
	camera.Track(cameraPosition, playerVector, 0);
	camera.Rotate(currentRotation.x, currentRotation.y, currentRotation.z);

	// FOV 업데이트
	GLOBAL.offsetFOV = std::lerp(GLOBAL.offsetFOV, destFOV, Delta * 20.0);
}

// 자기 소유의 총 객체에 위치와 회전각도를 전달한다.
void Player1st::updateGun() {
	// 1인칭 무기이므로 카메라 위치를 사용한다.
	if (weaponPtr) {
		weaponPtr->InputPosition(cameraPosition);
		weaponPtr->inputRotation(currentRotation);
		if(currentState == STATE_MOVE_SHOOT || currentState == STATE_MOVE)
			weaponPtr->inputMoveState(true);
		else
			weaponPtr->inputMoveState(false);
	}
}

// 플레이어 바운드 업데이트
void Player1st::updateBound() {
	if (currentState == STATE_DEATH) return;

	// 맵 - 플레이어 충돌 바운드
	playerSphere.Update(XMFLOAT3(playerPosition.x, playerPosition.y + 0.5, playerPosition.z), 1.0);

	// 몬스터 - 플레이어 충돌 바운드
	playerBound.Update(
		XMFLOAT3(playerPosition.x, playerPosition.y + playerSize.y * 0.5, playerPosition.z), 
		XMFLOAT3(playerSize.x * 0.5, playerSize.y, playerSize.z * 0.5), currentRotation
	);
}

void Player1st::updateIndicator() {
	if (weaponPtr && IndicatorPtr) {
		int currentAmmo = weaponPtr->getCurrentAmmo();
		int totalAmmo = weaponPtr->getTotalAmmo();
		IndicatorPtr->InputAmmo(totalAmmo, currentAmmo);
	}
}

//////////////////////////////////////////////////////


void Player1st::Update(float Delta) {
	updateState();
	updateMove(Delta);
	updateTerrainCollision();
	updateCamera(Delta);
	updateGun();
	updateBound();
	sendPacket(Delta);
	updateIndicator();
}

void Player1st::Render() {
	//playerBound.Render();
	//playerSphere.Render();
}

XMFLOAT3 Player1st::GetPosition() {
	return playerPosition;
}

OOBB Player1st::GetOOBB() {
	return playerBound;
}

XMFLOAT3 Player1st::GetSize() {
	return playerSize;
}

void Player1st::InputRecoil(float Value) {
	currentRotation.x -= Value;
}

void Player1st::GiveDamage(int damage) {
	if (currentState == STATE_DEATH) return;
	currentHP -= damage;
	Clamp::LimitValue(currentHP, 0, CLAMP_DIR_LESS);
	if (IndicatorPtr) IndicatorPtr->InputHP(totalHP, currentHP);



	// 체력이 0이 되면 상태를 죽음으로 변경한다.
	//if (currentHP == 0)
	//	currentState = STATE_DEATH;

	scene.AddObject(new PlayerHit, "playerHit", LAYERUI);
}
