#include "OtherPlayer.h"
#include "CameraUtil.h"
#include "ClampUtil.h"

// 캐릭터 타입에 따라 다른 fbx를 초기화 한다.
OtherPlayer::OtherPlayer(int characterType, unsigned int ID) {
	this->characterType = characterType;

	switch (this->characterType) {
	case CHARACTER_MG:
		for (int i = 0; i < 4; i++) 
			playerFBX[i].SelectFBXMesh(MESH.heavy[i]);

		totalHP = CHARACTER_MG_HP;
		currentHP = CHARACTER_MG_HP;
		break;

	case CHARACTER_DMR:
		for (int i = 0; i < 4; i++)
			playerFBX[i].SelectFBXMesh(MESH.marksman[i]);

		totalHP = CHARACTER_DMR_HP;
		currentHP = CHARACTER_DMR_HP;
		break;

	case CHARACTER_ENG:
		for (int i = 0; i < 4; i++)
			playerFBX[i].SelectFBXMesh(MESH.engineer[i]);

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
		playerFBX[currentState].ResetAnimation();
		prevState = currentState;
	}
}

void OtherPlayer::updateAnimation(float Delta) {
	switch (currentState) {
	case STATE_IDLE: case STATE_IDLE_SHOOT:
		playerFBX[STATE_IDLE].UpdateAnimation(Delta, false, !inFrustum);
		break;

	case STATE_MOVE: case STATE_MOVE_SHOOT:
		playerFBX[STATE_MOVE].UpdateAnimation(Delta, false, !inFrustum);
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
	if (playerFBX[3].GetAnimationEndState())
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

	RenderFBX(playerFBX[renderState], TEX.scifi);
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
