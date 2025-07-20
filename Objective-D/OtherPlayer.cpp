#include "OtherPlayer.h"
#include "CameraUtil.h"

// 캐릭터 타입에 따라 다른 fbx를 초기화 한다.
OtherPlayer::OtherPlayer(int characterType) {
	this->characterType = characterType;
}

void OtherPlayer::updateState() {
	if (!initState)
		return;

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
	if (!initState)
		return;

	switch (currentState) {
	case STATE_IDLE:
		idleFBX.UpdateAnimation(Delta, !inFrustum); break;
	case STATE_MOVE:
		moveFBX.UpdateAnimation(Delta, !inFrustum); break;
	case STATE_IDLE_SHOOT:
		shootFBX.UpdateAnimation(Delta, !inFrustum); break;
	case STATE_MOVE_SHOOT:
		moveFBX.UpdateAnimation(Delta, !inFrustum); break;
	}
}

void OtherPlayer::updateRenderValue(float Delta) {
	if (!initState)
		return;

	Math::LerpXMFLOAT3(position, positionDest, 10.0, Delta);
	Math::LerpXMFLOAT3(rotation, rotationDest, 10.0, Delta);
}

void OtherPlayer::updateBound() {
	if (!initState)
		return;

	frustumAABB.Update(position, size);
	inFrustum = camera.CheckFrustum(frustumAABB);
}

void OtherPlayer::Update(float Delta) {
	updateState();
	updateRenderValue(Delta);
	updateBound();

	if (!initState) {
		switch (characterType) {
		case CHARACTER_MG:
			idleFBX.SelectFBXMesh(MESH.heavyIdle);
			moveFBX.SelectFBXMesh(MESH.heavyMove);
			shootFBX.SelectFBXMesh(MESH.heavyShoot);
			deathFBX.SelectFBXMesh(MESH.heavyDeath);
			break;
		}
	}
}

void OtherPlayer::Render() {
	if (!initState || !inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(ScaleMatrix, size);
	Transform::Rotate(RotateMatrix, rotation);

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
	if (!initState)
		return;

	positionDest = position;
}

void OtherPlayer::InputRotation(XMFLOAT3& rotation) {
	if (!initState)
		return;

	rotationDest = rotation;
}

void OtherPlayer::InputState(unsigned int state) {
	if (!initState)
		return;

	currentState = state;
}
