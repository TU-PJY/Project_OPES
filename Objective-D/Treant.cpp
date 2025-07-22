#include "Treant.h"
#include "CameraUtil.h"
#include "HP_Indicator.h"

Treant::Treant(const xmfloat3& createPosition, unsigned int ID, bool defenseModeState) {
	position = createPosition;
	this->defenseMoveState = defenseModeState;
	this->ID = ID;

	treantOOBB.SetUpdateFrequency(24);

	hpInd = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER3);
	if (hpInd) hpInd->SetSize(1.5);
}

Treant::~Treant() {
	if (hpInd)
		scene.DeleteObject(hpInd);
}

void Treant::updateIndicator() {
	if (hpInd) {
		hpInd->InputPosition(position, 12.0);
		hpInd->InputHP(totalHP, currentHP);
	}
}

void Treant::updateState() {
	if (currentState != prevState) {
		treantFBX[currentState].ResetAnimation();
		prevState = currentState;
	}
}

void Treant::updateAnimation(float Delta) {
	treantFBX[currentState].UpdateAnimation(Delta, false, !inFrustum);
	treantOOBB.UpdateDelta(Delta);
}

void Treant::updateTerrainCollision() {
	terrainUtil.InputPosition(position);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.0);
}

void Treant::updateBound() {
	xmfloat3 boundPosition = xmfloat3(position.x, position.y + size.y * 1.5, position.z);
	frustumBound.Update(boundPosition, 15.0);
	inFrustum = camera.CheckFrustum(frustumBound);
}

void Treant::Update(float Delta) {
	updateTerrainCollision();
	updateIndicator();
	updateBound();
	updateState();
	updateAnimation(Delta);
}

void Treant::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	RenderFBX(treantFBX[renderState], TEX.treant);
	treantOOBB.UpdateAnimated(treantFBX[renderState], TranslateMatrix, RotateMatrix, ScaleMatrix, 0);
	renderState = currentState;

	//frustumBound.Render();
	//treantOOBB.Render();
}
