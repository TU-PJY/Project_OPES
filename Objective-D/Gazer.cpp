#include "Gazer.h"
#include "CameraUtil.h"

Gazer::Gazer(const xmfloat3& createPosition, unsigned int ID) {
	position = createPosition;
	hitBox.SetUpdateFrequency(24);
}

void Gazer::gz_updateBound() {
	xmfloat3 boundPosition = xmfloat3(position.x, position.y + size.y * 2.0, position.z);
	frustumBound.Update(boundPosition, 10.0);
	inFrustum = camera.CheckFrustum(frustumBound);
}

void Gazer::gz_updateAnimation(float Delta) {
	if(currentState == GAZER_WALK)
		gazerFBX.UpdateAnimation(Delta, true, !inFrustum);
	else
		gazerFBX.UpdateAnimation(Delta, false, !inFrustum);

	hitBox.UpdateDelta(Delta);
}

void Gazer::gz_updateTerrainCollision() {
	Ray playerRay = Math::CalcRayVector(xmfloat3(position.x, position.y + 40.0, position.z), xmfloat3(position.x, position.y - 40.0, position.z));
	float Distance;
	xmfloat3 newPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, playerRay.Origin, playerRay.Direction, Distance);
	position.y = newPosition.y - 2.0;
}

void Gazer::gz_updateState() {
	if (prevState != currentState) {
		switch (currentState) {
		case GAZER_IDLE:
			gazerFBX.SelectAnimation("Idle");
			break;

		case GAZER_WALK:
			gazerFBX.SelectAnimation("MoveForward");
			break;

		case GAZER_ATTACK:
			gazerFBX.SelectAnimation("Bash01");
			break;

		case GAZER_DEATH:
			gazerFBX.SelectAnimation("Death");
			break;
		}

		prevState = currentState;
	}
}

void Gazer::Update(float Delta) {
	gz_updateState();
	gz_updateTerrainCollision();
	gz_updateBound();
	gz_updateState();
	gz_updateAnimation(Delta);
}

void Gazer::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	if(currentState == GAZER_WALK)
		Transform::Move(TranslateMatrix, -gazerFBX.GetInplaceDelta());
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	RenderFBX(gazerFBX, TEX.gazer);
	hitBox.UpdateAnimated(gazerFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 4);

	frustumBound.Render();
	hitBox.Render();
}

OOBB Gazer::GetOOBB() {
	return hitBox;
}

bool Gazer::CheckHit(BoundSphere& sphere) {
	if (hitBox.CheckCollision(sphere))
		return true;
	return false;
}

xmfloat3 Gazer::GetPosition() {
	return position;
}

void Gazer::GiveDamage(int damage) {
	if (currentState == GAZER_DEATH)
		return;

	// packet
}

void Gazer::InputHP(int hp) {
	if (currentState == GAZER_DEATH)
		return;

	currentHP = hp;
}

bool Gazer::GetDeathState() {
	if (currentState == GAZER_DEATH)
		return true;
	return false;
}
