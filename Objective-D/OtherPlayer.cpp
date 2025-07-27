#include "OtherPlayer.h"
#include "CameraUtil.h"
#include "ClampUtil.h"
#include "ScriptUtil.h"

// 캐릭터 타입에 따라 다른 fbx를 초기화 한다.
OtherPlayer::OtherPlayer(int characterType, unsigned int ID) {
	this->characterType = characterType;
	ScriptUtil script{};
	script.Load("Resources//Scripts//weapon//flamePosition.xml");

	

	switch (this->characterType) {
	case CHARACTER_MG:
		idleFBX.SelectFBXMesh(MESH.heavy[0]);
		moveFBX.SelectFBXMesh(MESH.heavy[1]);
		shootFBX.SelectFBXMesh(MESH.heavy[2]);
		deathFBX.SelectFBXMesh(MESH.heavy[3]);

		totalHP = CHARACTER_MG_HP;
		currentHP = CHARACTER_MG_HP;

		flameIdlex = script.LoadDigitData("heavyIdle", "x");
		flameIdley = script.LoadDigitData("heavyIdle", "y");
		flameIdlez = script.LoadDigitData("heavyIdle", "z");

		flameMovex = script.LoadDigitData("heavyMove", "x");
		flameMovey = script.LoadDigitData("heavyMove", "y");
		flameMovez = script.LoadDigitData("heavyMove", "z");

		shootSound.Load("Resources//Sounds//machingunShoot.wav", FMOD_3D);
		break;

		// 소음기가 장착된 총이므로 불꽃을 렌더링하지 않는다.
	case CHARACTER_DMR:
		idleFBX.SelectFBXMesh(MESH.marksman[0]);
		moveFBX.SelectFBXMesh(MESH.marksman[1]);
		shootFBX.SelectFBXMesh(MESH.marksman[2]);
		deathFBX.SelectFBXMesh(MESH.marksman[3]);
		moveFBX.SetSpeed(2.0);

		totalHP = CHARACTER_DMR_HP;
		currentHP = CHARACTER_DMR_HP;

		shootSound.Load("Resources//Sounds//dmrShoot.mp3", FMOD_3D);
		break;

	case CHARACTER_ENG:
		idleFBX.SelectFBXMesh(MESH.engineer[0]);
		moveFBX.SelectFBXMesh(MESH.engineer[1]);
		shootFBX.SelectFBXMesh(MESH.engineer[2]);
		deathFBX.SelectFBXMesh(MESH.engineer[3]);

		totalHP = CHARACTER_ENG_HP;
		currentHP = CHARACTER_ENG_HP;

		flameIdlex = script.LoadDigitData("engineerIdle", "x");
		flameIdley = script.LoadDigitData("engineerIdle", "y");
		flameIdlez = script.LoadDigitData("engineerIdle", "z");

		flameMovex = script.LoadDigitData("engineerMove", "x");
		flameMovey = script.LoadDigitData("engineerMove", "y");
		flameMovez = script.LoadDigitData("engineerMove", "z");

		shootSound.Load("Resources//Sounds//shotgunShoot.mp3", FMOD_3D);
		break;
	}

	TerrainUtil terrainUtil;
	terrainUtil.InputPosition(position);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, positionDest, 0.0);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.0);

	this->ID = ID;
}

void OtherPlayer::updateState() {
	if (prevState != currentState) {
		switch (currentState) {
		case STATE_IDLE:
			idleFBX.ResetAnimation();
			break;

		case STATE_MOVE:
			moveFBX.ResetAnimation();
			break;

		case STATE_DEATH:
			deathFBX.ResetAnimation();
			break;
		}

		prevState = currentState;
	}
}

void OtherPlayer::updateAnimation(float Delta) {
	switch (currentState) {
	case STATE_IDLE: case STATE_IDLE_SHOOT:
		idleFBX.UpdateAnimation(Delta, false, !inFrustum);
		break;

	case STATE_MOVE: case STATE_MOVE_SHOOT:
		moveFBX.UpdateAnimation(Delta, false, !inFrustum);
		break;
	}
}

void OtherPlayer::updateRenderValue(float Delta) {
	Math::LerpXMFLOAT3(position, positionDest, 15.0, Delta);
	Math::LerpXMFLOAT3(rotation, rotationDest, 15.0, Delta);
}

void OtherPlayer::updateBound() {
	frustumBound.Update(position, 10.0);
	inFrustum = camera.CheckFrustum(frustumBound);
	playerBound.Update(
		XMFLOAT3(position.x, position.y + size.y * 1.5, position.z),
		XMFLOAT3(size.x * 0.5, size.y, size.z * 0.5), rotation
	);
}

void OtherPlayer::updateDeath() {
	if (currentState != STATE_DEATH)
		return;

	// 사망 시 사망 애니메이션이 끝난 후 삭제된다.
	if (deathFBX.GetAnimationEndState())
		scene.DeleteObject(this);
}

void OtherPlayer::Update(float Delta) {
	updateState();
	updateAnimation(Delta);
	updateRenderValue(Delta);
	updateBound();
	flameRenderTime -= Delta;
	Clamp::LimitValue(flameRenderTime, 0.0, CLAMP_DIR_LESS);
	shootSound.SetPosition(position);
}

void OtherPlayer::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(ScaleMatrix, size);
	Transform::Rotate(RotateMatrix, 0.0, rotation.y, 0.0);

	switch (renderState) {
	case STATE_IDLE: case STATE_IDLE_SHOOT:
		RenderFBX(idleFBX, TEX.scifi);
		RenderIdleFlame();
		break;

	case STATE_MOVE: case STATE_MOVE_SHOOT:
		RenderFBX(moveFBX, TEX.scifi);
		RenderMoveFlame();
		break;

	case STATE_DEATH:
		RenderFBX(deathFBX, TEX.scifi);
		break;
	}


	renderState = currentState;
}

void OtherPlayer::RenderIdleFlame() {
	if (characterType != CHARACTER_DMR) {
		if (flameRenderTime > 0.0) {
			BeginRender();
			SetLightUse(DISABLE_LIGHT);
			Transform::Move(TranslateMatrix, position);
			Transform::Rotate(TranslateMatrix, 0.0, rotation.y, 0.0);
			Transform::Move(TranslateMatrix, flameIdlex, flameIdley, flameIdlez);
			Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
			Render3D(MESH.gun_flame, TEX.gun_flame);
			Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
		}
	}
}

void OtherPlayer::RenderMoveFlame() {
	if (characterType != CHARACTER_DMR) {
		if (flameRenderTime > 0.0) {
			BeginRender();
			SetLightUse(DISABLE_LIGHT);
			Transform::Move(TranslateMatrix, position);
			Transform::Rotate(TranslateMatrix, 0.0, rotation.y, 0.0);
			Transform::Move(TranslateMatrix, flameMovex, flameMovey, flameMovez);
			Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
			Render3D(MESH.gun_flame, TEX.gun_flame);
			Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
		}
	}
}

void OtherPlayer::InputPosition(XMFLOAT3& position) {
	positionDest = position;
}

void OtherPlayer::InputRotation(XMFLOAT3& rotation) {
	rotationDest = rotation;
}

void OtherPlayer::InputState(unsigned int state) {
	currentState = state;
}

XMFLOAT3 OtherPlayer::GetPosition() {
	XMFLOAT3 outPosition = XMFLOAT3(position.x, position.y + size.y * 0.5, position.z);
	return outPosition;
}

void OtherPlayer::InputHP(int currentHP) {
	if (currentState == STATE_DEATH)
		return;

	//this->currentHP = currentHP;
	Clamp::LimitValue(this->currentHP, 0, CLAMP_DIR_LESS);
	if (this->currentHP == 0) {
		currentState = STATE_DEATH;
	}
}

OOBB OtherPlayer::GetOOBB() {
	return playerBound;
}

// 아무 역할 안하는 더미 함수
void OtherPlayer::GiveDamage(int damage)
{}

unsigned int OtherPlayer::GetID() {
	return ID;
}

void OtherPlayer::addFlameTime(){
	flameRenderTime = 0.05;

	switch (characterType) {
	case CHARACTER_MG:
		SOUND.machinegunShoot.Play3D(); break;

	case CHARACTER_DMR:
		SOUND.dmrShoot.Play3D(); break;

	case CHARACTER_ENG:
		SOUND.shotgunShoot.Play3D(); break;
	}
}
