#include "Player.h"
#include "Scene.h"
#include "MouseUtil.h"
#include "CameraController.h"
#include "ClampUtil.h"
#include "PickingUtil.h"
#include "Bullet.h"
#include "PlayerHit.h"

std::default_random_engine rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(0, 1);

void SendMovePacket(float x, float y,float z);
void SendViewingAnglePacket(float x, float y, float z);
void SendAnimaionPacket(unsigned short playerState);

// 생성자에서 입력받은 맵 오브젝트 이름으로 터레인 값을 받아온다.
Player::Player(std::string MapObjectName) {
	currentTerrainName = MapObjectName;
	mouse.StartMotionCapture(GlobalHWND);

	// 현재 맵에서 벽 oobb를 얻어온다.
	if (auto Map = scene.Find(MapObjectName); Map)
		map_oobb_data = Map->GetMapWallOOBB();

	if (auto building = scene.Find("center_building"); building)
		map_oobb_data.emplace_back(building->GetOOBB());

	// 오버헤드 감소를 위해 미리 크로스헤어 오브젝트 포인터를 저장한다.
	if (auto Object = scene.Find("crosshair"))
		crosshair = Object;

	playerBound.SetUpdateFrequency(24);
}

void Player::InputMouseMotion(MotionEvent& Event) {
	if (GetCapture() == Event.CaptureState) {
		mouse.HideCursor();
		GetCapture();

		// 정조준 시 감도를 절반으로 낮춘다
		float sensivity = 0.08;
		if (gunZoomState)  sensivity = 0.04;
		XMFLOAT2 Delta = mouse.GetMotionDelta(Event.Motion, sensivity);
		UpdateMotionRotation(rotation, Delta.x, Delta.y);
	}
}

void Player::InputMouse(MouseEvent& Event) {
	// 총 발사 상태 활성화 / 비활성화
	switch (Event.Type) {
	case WM_LBUTTONDOWN:
		// 마우스 모션 캡쳐 상태가 해제된 경우(윈도우 버튼 등으로 다른 윈도우에 포커싱된 경우)
		// 원래의 윈도우에 좌클릭으로 포커싱하면 모션 캡쳐 상태가 다시 활성화 된다.
		mouse.StartMotionCapture(GlobalHWND);
		triggerState = true;
		break;

	case WM_LBUTTONUP:
		triggerState = false;
		break;

	case WM_RBUTTONDOWN:
		gunZoomState = true;
		fovDest = -20.0;
		crosshair->DisableRender();
		break;

	case WM_RBUTTONUP:
		gunZoomState = false;
		fovDest = 0.0;
		crosshair->EnableRender();
		break;
	}
}

void Player::InputKey(KeyEvent& Event) {
	// 각 움직임 변수에 대응된 키
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'W', moveFrontState);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'S', moveBackState);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'A', moveLeftState);
	InputBoolSwitch(KEY_DOWN_TRUE, Event, 'D', moveRightState);
}

// 현재 위치, y회전값, x회전값, 대미지, 현재 맵 이름을 전달한다.
void Player::createBulletObject() {
	//scene.AddObject(new Bullet(cameraPosition, rotation.y, rotation.x, 10), "bullet", LAYER1);
}

// 서버 부하를 방지하기 위해 0.05초 간격으로 패킷 전송 
// 개발을 위해 잠시 비활성화
void Player::SendPacket(float Delta) {
	std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;

	sendDelay += Delta;

	if (sendDelay >= 0.025) {
		if (sendOrder == 1) {
			if (currentPlayerState == STATE_MOVE || currentPlayerState == STATE_MOVE_SHOOT)
				SendMovePacket(playerPosition.x, playerPosition.y, playerPosition.z);
		}

		else if (sendOrder == 2) {
			if (prevRotation.x != rotation.x || prevRotation.y != rotation.y || prevRotation.z != rotation.z) {
				SendViewingAnglePacket(rotation.x, rotation.y, rotation.z);
				prevRotation = rotation;
			}
		}

		else if (sendOrder == 3)
			SendAnimaionPacket(currentServerState);

		sendOrder += 1;
		if (sendOrder > 3)
			sendOrder = 1;

		float over_time = 0.025 - sendDelay;
		sendDelay = over_time;
	}
}

void Player::Update(float FrameTime) {
	// 총 발사 업데이트
	// 이동 속도 가감속 업데이트
	UpdateMoveSpeed(FrameTime);

	// 터레인 충돌 처리 업데이트
	UpdateTerrainCollision(FrameTime);

	// 플레이어 바운딩 스페어 업데이트
	player_sphere.Update(XMFLOAT3(playerPosition.x, playerPosition.y + 0.5, playerPosition.z), 1.0);

	// 총 - 맵 오브젝트 충돌 처리 업데이트
	UpdateGunCollision();
	 
	// 총 업데이트
	UpdateGun(FrameTime);

	// 카메라 업데이트
	UpdateCamera(FrameTime);

	UpdateFire(FrameTime);

	// 플레이어 OOBB 업데이트
	updateBound(FrameTime);

	// 서버로 패킷 전송
	SendPacket(FrameTime);
}

void Player::Render() {
	// 1인칭 총 렌더링
	BeginRender();
	Transform::Move(TranslateMatrix, cameraPosition);
	Transform::Rotate(TranslateMatrix, gunRotation.x, gunRotation.y + gunRotationOffset, gunRotation.z);
	Transform::Move(TranslateMatrix, gunPositionOffset.x, gunPositionOffset.y, gunPositionOffset.z + gunOffset);
	Render3D(MESH.machine_gun, TEX.scifi);

	// 레드 도트 렌더링
	SetLightUse(DISABLE_LIGHT);
	Render3D(MESH.dot_machine_gun, TEX.scifi);

	// flame_time 동안 불꽃 렌더링
	if (flameTime > 0.0) {
		// 불꽃 렌더링
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
	}
	gunOOBB.Update(MESH.machine_gun, TranslateMatrix, RotateMatrix, ScaleMatrix, true);
	gunOOBB.Render();
	player_sphere.Render();

	// 플레이어 바운드박스 업데이트
	BeginRender();
	Transform::Move(TranslateMatrix, playerPosition);
	Transform::Rotate(RotateMatrix, XMFLOAT3(0.0, rotation.y, 0.0));
	Transform::Scale(ScaleMatrix, XMFLOAT3(2.0, 2.0, 2.0));
	playerBound.UpdateAnimated(playerFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 0);
	playerBound.Render();
}

void Player::UpdateMoveSpeed(float FrameTime) {
	// 움직임 활성화 시 해당 방향으로 가속
	if (moveFrontState && !moveBackState)
		forwardSpeed = std::lerp(forwardSpeed, destMoveSpeed, 10.0 * FrameTime);
	if (moveBackState && !moveFrontState)
		forwardSpeed = std::lerp(forwardSpeed, -destMoveSpeed, 10.0 * FrameTime);
	if (moveRightState && !moveLeftState)
		strafeSpeed = std::lerp(strafeSpeed, destMoveSpeed, 10.0 * FrameTime);
	if (moveLeftState && !moveRightState)
		strafeSpeed = std::lerp(strafeSpeed, -destMoveSpeed, 10.0 * FrameTime);

	// 움직임 비활성화 또는 서로 반대 방향 이동 활성화 시 감속
	if ((!moveFrontState && !moveBackState) || (moveFrontState && moveBackState)) 
		forwardSpeed = std::lerp(forwardSpeed, 0.0, 10.0 * FrameTime);
	
	if ((!moveRightState && !moveLeftState) || (moveRightState && moveLeftState)) 
		strafeSpeed = std::lerp(strafeSpeed, 0.0, 10.0 * FrameTime);

	if ((moveFrontState && !moveBackState) || (moveBackState && !moveFrontState) ||
		(moveRightState && !moveLeftState) || (moveLeftState && !moveRightState)) {
		if (triggerState)
			currentPlayerState = STATE_MOVE_SHOOT;
		else
			currentPlayerState = STATE_MOVE;
	}
	else {
		if (triggerState)
			currentPlayerState = STATE_IDLE_SHOOT;
		else
			currentPlayerState = STATE_IDLE;
	}

	// OOBB와 충돌을 체크하면서 이동
	Math::MoveWithSlide(cameraPosition, rotation.y, forwardSpeed, strafeSpeed, player_sphere, map_oobb_data, FrameTime);
	playerPosition.x = cameraPosition.x;
	playerPosition.z = cameraPosition.z;
	
	currentServerState = currentPlayerState;
}

void Player::UpdateFire(float FrameTime) {
	// 총 발사 간격을 업데이트 한다.
	// dest_fire_delay 간격으로 발사하게 된다.
	if (currentFireDelay > 0.0)
		currentFireDelay -= FrameTime;

	if (flameTime > 0.0)
		flameTime -= FrameTime;

	// 발사 상태에서 current_fire_delay가 0.0이 되면 crosshair에 반동값 부여 -> 발사
	if (triggerState) {
		if (currentFireDelay <= 0.0) {
			// 총알 객체를 생성한다.
			createBulletObject();

			currentFireDelay = destFireDelay;
			crosshair->InputRecoil(0.1);

			gunOffset -= 0.1;
			int randNum = dist(gen);
			if (randNum == 1)
				destRecoilShake = 30.0;
			else
				destRecoilShake = -30.0;

			rotation.x -= 2.5;

			flameTime = 0.03;
		}
	}
}

void Player::UpdateGun(float FrameTime) {
	gunOffset = std::lerp(gunOffset, 0.0, FrameTime * 10.0);

	if (gunZoomState && !gunCollideState) {
		gunPositionOffset.x = std::lerp(gunPositionOffset.x, 0.0, FrameTime * 20.0);
		gunPositionOffset.y = std::lerp(gunPositionOffset.y, -0.22, FrameTime * 20.0);
		gunPositionOffset.z = std::lerp(gunPositionOffset.z, 0.4, FrameTime * 20.0);
	}

	else {
		gunPositionOffset.x = std::lerp(gunPositionOffset.x, 0.3, FrameTime * 20.0);
		gunPositionOffset.y = std::lerp(gunPositionOffset.y, -0.3, FrameTime * 20.0);
		gunPositionOffset.z = std::lerp(gunPositionOffset.z, 0.4, FrameTime * 20.0);
	}

	gunRotation.x = std::lerp(gunRotation.x, rotation.x, FrameTime * 30.0);
	gunRotation.y = std::lerp(gunRotation.y, rotation.y, FrameTime * 30.0);
	gunRotation.z = std::lerp(gunRotation.z, rotation.z, FrameTime * 30.0);

	if (gunCollideState)
		gunRotationOffset = std::lerp(gunRotationOffset, -90.0, FrameTime * 3.0);
	else 
		gunRotationOffset = std::lerp(gunRotationOffset, 0.0, FrameTime * 3.0);
}

void Player::UpdateTerrainCollision(float FrameTime) {
	// 플레이어 높이가 항상 터레인 위에 위치하도록 한다
	if (auto terrain = scene.Find(currentTerrainName); terrain) {
		terr.InputPosition(cameraPosition);
		terr.ClampToTerrain(terrain->GetTerrain(), cameraPosition, 3.0);
		terr.ClampToTerrain(terrain->GetTerrain(), playerPosition, 0.0);
	}
}

void Player::updateBound(float Delta) {
	playerBound.UpdateDelta(Delta);
	playerFBX.UpdateAnimation(Delta);
}

void Player::UpdateGunCollision() {
	for (auto const& O : map_oobb_data) {
		if (gunOOBB.CheckCollision(O)) {
			gunCollideState = true;
			return;
		}
	}

	gunCollideState = false;
}

void Player::UpdateCameraRotation() {
	// 상하 카메라 회전 제한
	Clamp::ClampValue(rotation.x, -90.0, 90.0, CLAMP_FIX);
	Clamp::ClampValue(rotation.z, -90.0, 90.0, CLAMP_FIX);

	// 벡터 및 카메라 추적 업데이트
	Math::UpdateVector(vec, rotation);
	camera.Track(cameraPosition, vec, 0);
}

void Player::UpdateWalkMotion(float FrameTime) {
	walkShakeNum += FrameTime * 10.0;

	// 걷기 상태 활성화 시 카메라가 흔들리는 연출을 준다.
	if (moveFrontState || moveBackState || moveRightState || moveLeftState)
		walkShakeValue = std::lerp(walkShakeValue, 1.5, FrameTime * 5.0);

	// 걷기 상태가 비활성화된 상태라면 점차 흔들림을 줄인다.
	else
		walkShakeValue = std::lerp(walkShakeValue, 0.0, FrameTime * 5.0);

	// 최종 흔들림 값 계산
	walkShakeResult = sinf(walkShakeNum) * walkShakeValue;
}

void Player::UpdateShootMotion(float FrameTime) {
	recoilShakeNum += FrameTime * 40.0;
	recoilShake = std::lerp(recoilShake, sinf(recoilShakeNum) * destRecoilShake, FrameTime * 5.0);

	if(triggerState)
		destRecoilShake = std::lerp(destRecoilShake, 0.0, 5.0 * FrameTime);
	else
		destRecoilShake = std::lerp(destRecoilShake, 0.0, 20.0 * FrameTime);
}

void Player::UpdateCamera(float FrameTime) {
	// 카메라 회전 업데이트
	UpdateCameraRotation();


	// 카메라 워킹은 임시로 비활성화
	// 걷기 모션 업데이트
	//UpdateWalkMotion(FrameTime);

	// 총 반동 연출 업데이트
	//UpdateShootMotion(FrameTime);

	// 카메라 최종 회전
	camera.Rotate(rotation.x, rotation.y, rotation.z + walkShakeResult + recoilShake);

	// 정조준 시 fov 업데이트
	//globalFovOffset = std::lerp(globalFovOffset, fovDest, FrameTime * 20.0);
}

void Player::GiveDamage(int HP) {
	// 대미지를 받으면 피드백을 표시한다.
	scene.AddObject(new PlayerHit, "playerHit", LAYER_UI2);
}
