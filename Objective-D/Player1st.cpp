#include "Player1st.h"
#include "PlayerHit.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ClampUtil.h"
#include "HeavyMachineGun.h"
#include "DMR.h"
#include "Shotgun.h"
#include "PlayerIndicator.h"
#include "Turret.h"
#include "Beacon.h"
#include "InstallIndicator.h"
#include "Scope.h"

#include "Grenade.h"

void SendMovePacket(float x, float y, float z);
void SendViewingAnglePacket(float x, float y, float z);
void SendAnimaionPacket(unsigned short playerState);
void SendMtoPDamagePacket(unsigned int playerID, unsigned int monsterID, int attackHp);

Player1st::Player1st(int characterType) {
	// 현재 캐릭터 타입에 맞는 무기 객체를 추가 후 연결한다.
	switch (characterType) {
	case CHARACTER_MG:
		weaponPtr = scene.AddObject(new HeavyMachineGun(this), "mg", LAYER4);
		maxSpeed = CHARACTER_MG_SPEED;
		totalHP = CHARACTER_MG_HP;
		currentHP = CHARACTER_MG_HP;
		break;

	case CHARACTER_DMR:
		weaponPtr = scene.AddObject(new DMR(this), "dmr", LAYER4);
		scopePtr = scene.AddObject(new Scope, "scope", LAYERUI);
		maxSpeed = CHARACTER_DMR_SPEED;
		totalHP = CHARACTER_DMR_HP;
		currentHP = CHARACTER_DMR_HP;
		break;

	case CHARACTER_ENG:
		weaponPtr = scene.AddObject(new Shotgun(this), "shotgun", LAYER4);
		maxSpeed = CHARACTER_ENG_SPEED;
		totalHP = CHARACTER_ENG_HP;
		currentHP = CHARACTER_ENG_HP;

		installPtr = scene.AddObject(new InstallIndicator, "inst", LAYER5);
		if (installPtr)
			installPtr->SetRenderState(false);
	
		break;
	}

	this->characterType = characterType;
	currentSpeed = maxSpeed;
	
	// 플레이어 인디케이터에 필요한 정보들을 전달한다.
	IndicatorPtr = scene.AddObject(new PlayerIndicator(this->characterType), "playerInd", LAYERUI);
	if (IndicatorPtr) {
		IndicatorPtr->InputHP(totalHP, currentHP);
		IndicatorPtr->InputGrenade(currentGrenadeCount);
		if (weaponPtr) {
			int totalAmmo = weaponPtr->getTotalAmmo();
			int currentAmmo = weaponPtr->getCurrentAmmo();
			IndicatorPtr->InputAmmo(totalAmmo, currentAmmo);
		}
	}

	// 2번 맵은 미끄러워서 가감속이 느려짐
	if (GLOBAL.mapName.compare("map2") == 0)
		speedAcc = 1.0;
	else
		speedAcc = 10.0;
}

Player1st::~Player1st() {
	if (weaponPtr)
		scene.DeleteObject(weaponPtr);

	if (IndicatorPtr)
		scene.DeleteObject(IndicatorPtr);

	if (installPtr)
		scene.DeleteObject(installPtr);
}

void Player1st::sendPacket(float Delta) {
	currentPacketSendDelay += Delta;
	if (currentPacketSendDelay >= packetSendDelay) {
		currentPacketSendDelay -= packetSendDelay;

		switch (sendOrder) {
		case 1:
			// map2는 미끄러지기 때문에 계속해서 보내줘야 한다.
			if (GLOBAL.mapName != "map2") {
				if (currentState == STATE_MOVE || currentState == STATE_MOVE_SHOOT)
					SendMovePacket(playerPosition.x, playerPosition.y, playerPosition.z);
			}
			else
				SendMovePacket(playerPosition.x, playerPosition.y, playerPosition.z);
			break;

		case 2:
			if (prevRotation.y != currentRotation.y) {
				SendViewingAnglePacket(currentRotation.x, currentRotation.y, currentRotation.z);
				prevRotation = currentRotation;
			}
			break;

		case 3:
			if (serverState != currentState) {
				SendAnimaionPacket(currentState);
				serverState = currentState;
			}
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
		// 지정사수의 경우 감도를 더 낮춘다
		float sensivity = 0.08;
		if (zoomState) {
			if(characterType == CHARACTER_DMR)
				sensivity = 0.02;
			else
				sensivity = 0.04;
		}
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
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(Event.wParam); // 휠 스크롤량 (+120, -120 등)

		if (delta > 0) {
			IndicatorPtr->ScrollRight();
			int index = IndicatorPtr->GetCurrentIndex();

			if (characterType == CHARACTER_ENG) {
				if (index < 3) {
					if (index == 0 && turretCoolTime <= 0.0)
						installPtr->SetRenderState(true);

					else if (index == 1 && beaconCoolTime <= 0.0)
						installPtr->SetRenderState(true);

					else
						installPtr->SetRenderState(false);

					installPtr->SetItem(index);
				}
				else
					installPtr->SetRenderState(false);
			}

			else if (characterType == CHARACTER_DMR)
				scopePtr->SetRenderState(false);
		}


		else {
			IndicatorPtr->ScrollLeft();
			int index = IndicatorPtr->GetCurrentIndex();

			if (characterType == CHARACTER_ENG) {
				if (index < 3) {
					if (index == 0 && turretCoolTime <= 0.0)
						installPtr->SetRenderState(true);

					else if (index == 1 && beaconCoolTime <= 0.0)
						installPtr->SetRenderState(true);

					else
						installPtr->SetRenderState(false);

					installPtr->SetItem(index);
				}
				else
					installPtr->SetRenderState(false);
			}

			else if (characterType == CHARACTER_DMR)
				scopePtr->SetRenderState(false);
		}


		if (weaponPtr) weaponPtr->releaseTrigger();
		triggerState = false;


		if (weaponPtr) weaponPtr->disableZoom();
		zoomState = false;
		destFOV = 0.0;
		currentSpeed = maxSpeed;
	}
	break;

	// 창 위에서 움직이면 자동 포커싱 된다.
	case WM_MOUSEMOVE:
		mouse.StartMotionCapture(GlobalHWND);
		break;

	case WM_LBUTTONDOWN:
		// 마우스 모션 캡쳐 상태가 해제된 경우(윈도우 버튼 등으로 다른 윈도우에 포커싱된 경우)
		// 원래의 윈도우에 좌클릭으로 포커싱하면 모션 캡쳐 상태가 다시 활성화 된다.
		mouse.StartMotionCapture(GlobalHWND);

		if (characterType == CHARACTER_MG || characterType == CHARACTER_DMR) {
			switch (IndicatorPtr->GetCurrentIndex()) {
			case 0:
				if (currentGrenadeCount > 0) {
					XMFLOAT3 rotation = XMFLOAT3(-currentRotation.x, currentRotation.y, currentRotation.z);
					XMFLOAT3 createPosition = cameraPosition;
					Math::CalcForwardOffset(createPosition, currentRotation.y, 2.0, 0.0);
					scene.AddObject(new Grenade(createPosition, rotation), "grenade", LAYER3);
					currentGrenadeCount--;
				}
				break;
			
			case 1:
				if (weaponPtr) weaponPtr->pullTrigger();
				triggerState = true;
				break;
			}
		}

		else {
			switch (IndicatorPtr->GetCurrentIndex()) {
			case 0:
				if (turretCoolTime <= 0.0) {
					float distance;
					xmfloat3 createPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, distance);
					if (distance > 8.0)
						break;
					scene.AddObject(new Turret(createPosition, currentRotation.y, false), "turret", LAYER3);
					turretCoolTime = TURRET_INSTALL_COOLTIME;
					installPtr->SetRenderState(false);
				}
				break;

			case 1:
				if (beaconCoolTime <= 0.0) {
					float distance;
					xmfloat3 createPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, distance);
					if (distance > 8.0)
						break;
					scene.AddObject(new Beacon(createPosition, currentRotation.y), "beacon", LAYER3);
					beaconCoolTime = BEACON_INSTALL_COOLTIME;
					installPtr->SetRenderState(false);
				}
			
			break;

			case 2:
				break;

			case 3:
				if (weaponPtr) weaponPtr->pullTrigger();
				triggerState = true;
				break;
			}
		}
		break;

	case WM_LBUTTONUP:
		if (weaponPtr) weaponPtr->releaseTrigger();
		triggerState = false;
		break;

	case WM_RBUTTONDOWN:
		mouse.StartMotionCapture(GlobalHWND);

		if ((characterType == CHARACTER_MG || characterType == CHARACTER_DMR) && IndicatorPtr->GetCurrentIndex() != 1)
			break;
		if (characterType == CHARACTER_ENG && IndicatorPtr->GetCurrentIndex() != 3)
			break;

		if (weaponPtr && !weaponPtr->getReloadState()) {
			weaponPtr->enableZoom();
			zoomState = true;
			currentSpeed = maxSpeed * 0.5;

			if (characterType == CHARACTER_DMR) {
				destFOV = -45.0;
				scopePtr->SetRenderState(true);
			}
			else
				destFOV = -20.0;
		}
		
		break;

	case WM_RBUTTONUP:
		if (weaponPtr) weaponPtr->disableZoom();
		zoomState = false;
		destFOV = 0.0;
		currentSpeed = maxSpeed;
		if (characterType == CHARACTER_DMR)
			scopePtr->SetRenderState(false);
		break;
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

					if (characterType == CHARACTER_DMR)
						scopePtr->SetRenderState(false);
				}
			}
			break;

		break;
		}
	}
}

// 상태를 업데이트 한다.
void Player1st::updateState() {
	//if (currentState == STATE_DEATH) return;

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
//	if (currentState == STATE_DEATH) return;

	// 앞뒤 가속/감속
	if (moveState[FRONT] && !moveState[BACK])
		forwardSpeed = std::lerp(forwardSpeed, currentSpeed, speedAcc * Delta);
	else if (!moveState[FRONT] && moveState[BACK])
		forwardSpeed = std::lerp(forwardSpeed, -currentSpeed, speedAcc * Delta);
	else 
		forwardSpeed = std::lerp(forwardSpeed, 0.0, speedAcc * Delta);

	// 좌우 가속/감속
	if (moveState[RIGHT] && !moveState[LEFT])
		strafeSpeed = std::lerp(strafeSpeed, currentSpeed, speedAcc * Delta);
	else if (!moveState[RIGHT] && moveState[LEFT])
		strafeSpeed = std::lerp(strafeSpeed, -currentSpeed, speedAcc * Delta);
	else
		strafeSpeed = std::lerp(strafeSpeed, 0.0, speedAcc * Delta);

	// 맵 바운드와 충돌을 체크하면서 이동
	Math::MoveWithSlide(playerPosition, currentRotation.y, forwardSpeed, strafeSpeed, playerSphere, GLOBAL.mapOOBBdata, Delta);

	// 맵 3한정으로 땅 바깥으로 나가면 떨어진다.
	if (fallDown) {
		fallAcc += Delta * 0.5;
		playerPosition.y -= fallAcc;
	}

	// 카메라 위치를 플레이어 위치와 동기화 
	cameraPosition = playerPosition;
}

// 플레이어 위치 - 터레인 충돌 처리를 업데이트 한다.
void Player1st::updateTerrainCollision() {
	terrainUtil.InputPosition(playerPosition);

	if (GLOBAL.mapName.compare("map3") != 0) {
		terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, playerPosition, 0.0);

		// 카메라의 경우 플레이어 모델의 실제 눈 높이에 위치하도록 한다.
		terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, cameraPosition, playerSize.y * 1.5);
	}

	// 맵3의 경우 다른 방식으로 터레인을 검사해야 한다.
	else {
		if (!fallDown) {
			Ray playerRay = Math::CalcRayVector(xmfloat3(playerPosition.x, playerPosition.y + 40.0, playerPosition.z), xmfloat3(playerPosition.x, playerPosition.y - 40.0, playerPosition.z));
			float Distance;
			xmfloat3 newPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, playerRay.Origin, playerRay.Direction, Distance);
			
			// 땅을 벗어나면 떨어진다
			if ((newPosition.x == 0.0 && newPosition.y == 0.0 && newPosition.z == 0.0) || newPosition.y <= -5.0) {
				cameraPosition.y = playerPosition.y + playerSize.y * 1.5;
				fallDown = true;
				return;
			}
			playerPosition.y = newPosition.y;
			cameraPosition.y = playerPosition.y + playerSize.y * 1.5;
		}
	}
}

// 카메라를 업데이트 한다.
void Player1st::updateCamera(float Delta) {
	//if (currentState == STATE_DEATH) return;

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
	//if (currentState == STATE_DEATH) return;

	// 맵 - 플레이어 충돌 바운드
	playerSphere.Update(XMFLOAT3(playerPosition.x, playerPosition.y + 0.5, playerPosition.z), 1.0);

	// 몬스터 - 플레이어 충돌 바운드
	playerBound.Update(
		XMFLOAT3(playerPosition.x, playerPosition.y + playerSize.y * 0.5, playerPosition.z), 
		XMFLOAT3(playerSize.x * 0.5, playerSize.y * 2.0, playerSize.z * 0.5), currentRotation
	);
}

void Player1st::updateIndicator() {
	if (weaponPtr && IndicatorPtr) {
		int currentAmmo = weaponPtr->getCurrentAmmo();
		int totalAmmo = weaponPtr->getTotalAmmo();
		IndicatorPtr->InputAmmo(totalAmmo, currentAmmo);
		IndicatorPtr->InputTurretCoolTime(turretCoolTime);
		IndicatorPtr->InputBeaconCoolTime(beaconCoolTime);
		IndicatorPtr->InputGrenade(currentGrenadeCount);
	}
}

//////////////////////////////////////////////////////


void Player1st::Update(float Delta) {
	turretCoolTime -= Delta;
	beaconCoolTime -= Delta;
	Clamp::LimitValue(turretCoolTime, 0.0, CLAMP_DIR_LESS);
	Clamp::LimitValue(beaconCoolTime, 0.0, CLAMP_DIR_LESS);

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

void Player1st::GiveHeal(int healHP) {
	if (currentState == STATE_DEATH)
		return;

	if (!GLOBAL.useServer) {
		currentHP += healHP;
		Clamp::LimitValue(currentHP, totalHP, CLAMP_DIR_GREATER);
		if (IndicatorPtr) IndicatorPtr->InputHP(totalHP, this->currentHP);
	}
	else {
		//음수를 보내 회복
		SendMtoPDamagePacket(GLOBAL.myID, 0, -5);
	}
}

void Player1st::GiveDamage(int damage) {
	if (currentState == STATE_DEATH) return;
	SendMtoPDamagePacket(GLOBAL.myID, 0, damage);

	if (!GLOBAL.useServer) {
		scene.AddObject(new PlayerHit, "playerHit", LAYERUI);
		currentHP -= damage;
		if (currentHP < 0)
			currentHP = 0;

		if (IndicatorPtr) IndicatorPtr->InputHP(totalHP, this->currentHP);
	}

	// 체력이 0이 되면 상태를 죽음으로 변경한다.
	if (currentHP == 0)
		currentState = STATE_DEATH;
}

void Player1st::InputHP(int currentHP) {
	if (currentState == STATE_DEATH) return;

	//this->currentHP = currentHP;
	Clamp::LimitValue(this->currentHP, 0, CLAMP_DIR_LESS);
	if (IndicatorPtr) 
		IndicatorPtr->InputHP(totalHP, this->currentHP);
	scene.AddObject(new PlayerHit, "playerHit", LAYERUI);
	std::cout << "PLAYER HP: " << this->currentHP << std::endl;
}

unsigned int Player1st::GetID() {
	return GLOBAL.myID;
}
