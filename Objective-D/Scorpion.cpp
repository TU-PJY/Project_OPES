#include "Scorpion.h"
#include "HP_Indicator.h"
#include "CameraUtil.h"

Scorpion::Scorpion(const XMFLOAT3& createPosition, const std::string& terrainName) {
	position = createPosition;
	currentMapName = terrainName;
	if (auto terrain = scene.Find(terrainName); terrain)
		currentTerrain = terrain;

	hpIndicator = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER2);

	for(int i = 0; i < 3; i++)
		hitBox[i].SetUpdateFrequency(30);
}

Scorpion::~Scorpion() {
	if (hpIndicator)
		scene.DeleteObject(hpIndicator);
}

void Scorpion::updateBound(float Delta) {
	frustumAABB.Update(XMFLOAT3(position.x, position.y + size.y, position.z), XMFLOAT3(size.x * 2.0, size.y * 0.8, size.z * 2.0));
	inFrustum = camera.CheckFrustum(frustumAABB);

	for (int i = 0; i < 3; i++)
		hitBox[i].UpdateDelta(Delta);
}

void Scorpion::updateIndicator() {
	hpIndicator->InputPosition(position, size.y);
	hpIndicator->InputHP(totalHP, currentHP);
}

void Scorpion::updateTerrain() {
	if (!inFrustum)
		return;

	if (currentTerrain) {
		terrainUtil.InputPosition(position);
		terrainUtil.ClampToTerrain(currentTerrain->GetTerrain(), position, 0.0);
	}
}

void Scorpion::updateState() {
	if (prevState != currentState) {
		switch (currentState) {
		case SCOR_IDLE:
			scorpionFBX.SelectAnimation("Idle"); break;
		case SCOR_WALK:
			scorpionFBX.SelectAnimation("Walk"); break;
		case SCOR_ATTACK:
			scorpionFBX.SelectAnimation("Attack 1"); break;
		case SCOR_DEATH:
			scorpionFBX.SelectAnimation("Death"); break;
		}

		prevState = currentState;
	}
}

void Scorpion::updateAnimation(float Delta) {
	if (currentState == SCOR_WALK)
		scorpionFBX.UpdateAnimation(Delta, true, !inFrustum);
	else
		scorpionFBX.UpdateAnimation(Delta, false, !inFrustum);
}

void Scorpion::Update(float Delta) {
	updateBound(Delta);
	updateTerrain();
	updateIndicator();
	updateState();
	updateAnimation(Delta);
}

void Scorpion::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	if (currentState == SCOR_WALK) {
		XMFLOAT3 inplaceDelta = scorpionFBX.GetInplaceDelta(size);
		Transform::Move(TranslateMatrix, -inplaceDelta);
	}
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	RenderFBX(scorpionFBX, TEX.scorpion);

	hitBox[0].UpdateAnimated(scorpionFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 8);
	hitBox[1].UpdateAnimated(scorpionFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 9);
	hitBox[2].UpdateAnimated(scorpionFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 12);

	for (int i = 0; i < 3; i++)
		hitBox[i].Render();

	frustumAABB.Render();

}
