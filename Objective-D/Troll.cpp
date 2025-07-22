#include "Troll.h"
#include "CameraUtil.h"
#include "HP_Indicator.h"

Troll::Troll(const xmfloat3& createPosition, unsigned int ID) {
	position = createPosition;
	this->ID = ID;
	trollOOBB.SetUpdateFrequency(24);

	hpInd = scene.AddObject(new HP_Indicator, "hpInd", LAYER3);
}

Troll::~Troll() {
	if (hpInd)
		scene.DeleteObject(hpInd);
}

void Troll::updateIndicator() {
	if (hpInd) {
		hpInd->InputPosition(position, 8.0);
		hpInd->InputHP(totalHP, currentHP);
	}
}

void Troll::updateTerrainCollision() {
	terrainUtil.InputPosition(position);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.0);
}

void Troll::updateState() {
	if (currentState != prevState) {
		switch (currentState) {
		case TROLL_IDLE:
			trollFBX.SelectAnimation("Idle");
			break;
		case TROLL_MOVE:
			trollFBX.SelectAnimation("Walk");
			break;
		case TROLL_ATTACK:
			trollFBX.SelectAnimation("Attack 3");
			break;
		case TROLL_DEATH:
			trollFBX.SelectAnimation("Death");
			break;
		}
		prevState = currentState;
	}
}

void Troll::updateAnimation(float Delta) {
	if(currentState != TROLL_MOVE)
		trollFBX.UpdateAnimation(Delta, false, !inFrustum);
	else
		trollFBX.UpdateAnimation(Delta, true, !inFrustum);

	trollOOBB.UpdateDelta(Delta);
}

void Troll::updateBound() {
	// 프러스텀 체크
	xmfloat3 boundPosition = xmfloat3(position.x, position.y + size.y, position.z);
	frustumBound.Update(boundPosition, 12.0);
	inFrustum = camera.CheckFrustum(frustumBound);
}

void Troll::Update(float Delta) {
	updateTerrainCollision();
	updateIndicator();
	updateBound();
	updateState();
	updateAnimation(Delta);
}

void Troll::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	if (currentState == TROLL_MOVE)
		Transform::Move(ScaleMatrix, -trollFBX.GetInplaceDelta());
	RenderFBX(trollFBX, TEX.troll);
	trollOOBB.UpdateAnimated(trollFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 3);

	//frustumBound.Render();
	//trollOOBB.Render();
}
