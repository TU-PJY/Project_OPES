#include "OtherPlayer.h"
#include "CameraUtil.h"
#include "ClampUtil.h"

// 캐릭터 타입에 따라 다른 fbx를 초기화 한다.
OtherPlayer::OtherPlayer(int characterType, unsigned int ID) {
	this->characterType = characterType;

	switch (this->characterType) {
	case CHARACTER_MG:
		idleFBX.SelectFBXMesh(MESH.heavy[0]);
		moveFBX.SelectFBXMesh(MESH.heavy[1]);
		shootFBX.SelectFBXMesh(MESH.heavy[2]);
		deathFBX.SelectFBXMesh(MESH.heavy[3]);

		totalHP = CHARACTER_MG_HP;
		currentHP = CHARACTER_MG_HP;
		break;

	case CHARACTER_DMR:
		idleFBX.SelectFBXMesh(MESH.marksman[0]);
		moveFBX.SelectFBXMesh(MESH.marksman[1]);
		shootFBX.SelectFBXMesh(MESH.marksman[2]);
		deathFBX.SelectFBXMesh(MESH.marksman[3]);

		totalHP = CHARACTER_DMR_HP;
		currentHP = CHARACTER_DMR_HP;
		break;

	case CHARACTER_ENG:
		idleFBX.SelectFBXMesh(MESH.engineer[0]);
		moveFBX.SelectFBXMesh(MESH.engineer[1]);
		shootFBX.SelectFBXMesh(MESH.engineer[2]);
		deathFBX.SelectFBXMesh(MESH.engineer[3]);

		totalHP = CHARACTER_ENG_HP;
		currentHP = CHARACTER_ENG_HP;
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
		break;

	case STATE_MOVE: case STATE_MOVE_SHOOT:
		RenderFBX(moveFBX, TEX.scifi);
		break;

	case STATE_DEATH:
		RenderFBX(deathFBX, TEX.scifi);
		break;
	}

	renderState = currentState;
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

	this->currentHP = currentHP;
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
