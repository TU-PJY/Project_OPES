#include "OtherPlayer.h"
#include "CameraUtil.h"
#include "ClampUtil.h"

// 캐릭터 타입에 따라 다른 fbx를 초기화 한다.
OtherPlayer::OtherPlayer(int characterType) {
	this->characterType = characterType;
	
	switch (this->characterType) {
	case CHARACTER_MG:
		idleFBX.SelectFBXMesh(MESH.heavyIdle);
		moveFBX.SelectFBXMesh(MESH.heavyMove);
		shootFBX.SelectFBXMesh(MESH.heavyShoot);
		deathFBX.SelectFBXMesh(MESH.heavyDeath);

		totalHP = 100;
		currentHP = 100;
		break;

	case CHARACTER_DMR:
		totalHP = 100;
		currentHP = 100;
		break;

	case CHARACTER_ENG:
		totalHP = 100;
		currentHP = 100;
		break;
	}

	TerrainUtil terrainUtil;
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, positionDest, 0.0);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.0);
}

void OtherPlayer::updateState() {
	if (prevState != currentState) {
		switch (currentState) {
		case STATE_IDLE:
			idleFBX.ResetAnimation(); break;
		case STATE_MOVE:
			moveFBX.ResetAnimation(); break;
		case STATE_IDLE_SHOOT:
			shootFBX.ResetAnimation(); break;
		case STATE_MOVE_SHOOT:
			moveFBX.ResetAnimation(); break;
		}

		prevState = currentState;
	}
}

void OtherPlayer::updateAnimation(float Delta) {
	switch (currentState) {
	case STATE_IDLE:
		idleFBX.UpdateAnimation(Delta, false, !inFrustum); break;
	case STATE_MOVE:
		moveFBX.UpdateAnimation(Delta, false, !inFrustum); break;
	case STATE_IDLE_SHOOT:
		shootFBX.UpdateAnimation(Delta, false, !inFrustum); break;
	case STATE_MOVE_SHOOT:
		moveFBX.UpdateAnimation(Delta, false, !inFrustum); break;
	}
}

void OtherPlayer::updateRenderValue(float Delta) {
	Math::LerpXMFLOAT3(position, positionDest, 20.0, Delta);
	Math::LerpXMFLOAT3(rotation, rotationDest, 20.0, Delta);
}

void OtherPlayer::updateBound() {
	frustumAABB.Update(position, size);
	inFrustum = camera.CheckFrustum(frustumAABB);
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
	case STATE_IDLE:
		RenderFBX(idleFBX, TEX.scifi); break;
	case STATE_MOVE:
		RenderFBX(moveFBX, TEX.scifi); break;
	case STATE_IDLE_SHOOT:
		RenderFBX(shootFBX, TEX.scifi); break;
	case STATE_MOVE_SHOOT:
		RenderFBX(moveFBX, TEX.scifi); break;
	}

	// 잔상 방지를 위해 실제 렌더링 되는 상태는 늦게 평가한다.
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
	return position;
}

void OtherPlayer::GiveDamage(int damage) {
	if (currentState == STATE_DEATH)
		return;

	currentHP -= damage;
	Clamp::LimitValue(currentHP, 0, CLAMP_DIR_LESS);
	if (currentHP == 0) {
		currentState = STATE_DEATH;
	}
}