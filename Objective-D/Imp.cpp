#include "Imp.h"
#include "CameraUtil.h"

Imp::Imp(const xmfloat3& createPosition, unsigned int ID) {
	position = createPosition;
	hitBox.SetUpdateFrequency(24);
}

void Imp::im_updateTerrainCollision() {
	Ray playerRay = Math::CalcRayVector(xmfloat3(position.x, position.y + 40.0, position.z), xmfloat3(position.x, position.y - 40.0, position.z));
	float Distance;
	xmfloat3 newPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, playerRay.Origin, playerRay.Direction, Distance);

	// ¶¥À» ¹þ¾î³ª¸é ¶³¾îÁø´Ù
	if ((newPosition.x == 0.0 && newPosition.y == 0.0 && newPosition.z == 0.0) || newPosition.y <= -5.0) {
		fallDown = true;
		return;
	}

	position.y = newPosition.y;
}

void Imp::im_updateBound() {
	xmfloat3 boundPosition = xmfloat3(position.x, position.y + size.y, position.z);
	frustumBound.Update(boundPosition, 10.0);
	inFrustum = camera.CheckFrustum(frustumBound);
	xmfloat3 boundPos = xmfloat3(position.x, position.y + size.y, position.z);
	boundPos = Math::CalcForwardOffset(boundPos, rotation.y, 2.0, 0.0);
	attackBound.Update(boundPos, xmfloat3(2.0, 2.0, 3.0), rotation);
}

void Imp::im_updateAnimation(float Delta) {
	if(currentState == IMP_WALK || currentState == IMP_ATTACK)
		impFBX.UpdateAnimation(Delta, true, !inFrustum);
	else
		impFBX.UpdateAnimation(Delta, false, !inFrustum);

	hitBox.UpdateDelta(Delta);
}

void Imp::im_updateState() {
	if (prevState != currentState) {
		switch (currentState) {
		case IMP_IDLE:
			impFBX.SelectAnimation("Idle");
			break;

		case IMP_WALK:
			impFBX.SelectAnimation("Walk");
			break;

		case IMP_ATTACK:
			impFBX.SelectAnimation("Attack02");
			break;

		case IMP_DEATH:
			impFBX.SelectAnimation("Death");
			break;
		}

		prevState = currentState;
	}
}

void Imp::im_updateAttack() {
	if (currentState != IMP_ATTACK) {
		attackDid = false;
		return;
	}

	if (impFBX.GetTimeSectionPassed(39.8)) {
		if (!attackDid) {
			if (currentTargetID == GLOBAL.myID) {
				if (auto player = scene.SearchLayer(LAYER_PLAYER, "player"); player) {
					player->GiveDamage(GAZER_DAMAGE);
				}
			}
			attackDid = true;
		}
	}
	else
		attackDid = false;
}

void Imp::Update(float Delta) {
	im_updateState();
	im_updateTerrainCollision();
	im_updateBound();
	im_updateState();
	im_updateAnimation(Delta);
	im_updateAttack();
}

void Imp::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	if (currentState == IMP_WALK || currentState == IMP_ATTACK)
		Transform::Move(ScaleMatrix, 0.0, 0.0, -impFBX.GetInplaceDelta().z);
	RenderFBX(impFBX, TEX.imp);
	hitBox.UpdateAnimated(impFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 3);

	//frustumBound.Render();
	//hitBox.Render();
	attackBound.Render();
}

OOBB Imp::GetOOBB() {
	return hitBox;
}

bool Imp::CheckHit(BoundSphere& sphere) {
	if (currentState == IMP_DEATH)
		return false;
	if (hitBox.CheckCollision(sphere))
		return true;
	return false;
	
}

xmfloat3 Imp::GetPosition() {
	return position;
}

void Imp::GiveDamage(int damage) {
	if (currentState == IMP_DEATH)
		return;

	// packet
}

void Imp::InputHP(int hp) {
	if (currentState == IMP_DEATH)
		return;

	currentHP = hp;
}

bool Imp::GetDeathState() {
	if (currentState == IMP_DEATH)
		return true;
	return false;
}
