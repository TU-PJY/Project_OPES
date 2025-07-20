#include "Scorpion.h"
#include "HP_Indicator.h"
#include "CameraUtil.h"
void SendMonstertypePacket(unsigned int monsterType, unsigned int monsterState, unsigned int id);

Scorpion::Scorpion(const XMFLOAT3& createPosition, const std::string& terrainName, unsigned int ID) {
	position = createPosition;
	currentMapName = terrainName;
	if (auto terrain = scene.Find(terrainName); terrain) {
		currentTerrain = terrain;
		mapBounds = terrain->GetMapWallOOBB();
	}

	hpIndicator = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER2);

	for(int i = 0; i < 3; i++)
		hitBox[i].SetUpdateFrequency(30);

	this->ID = ID;
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

	lookRange.Update(position, 60.0);
	scorBound.Update(XMFLOAT3(position.x, position.y + 0.5, position.z), 1.0);
	XMFLOAT3 attackBoundPosition = Math::CalcForwardOffset(position, rotation.y, 4.0, size.y * 0.5);
	attackBound.Update(attackBoundPosition, 3.0);
}

void Scorpion::updateIndicator() {
	hpIndicator->InputPosition(position, size.y * 1.7);
	hpIndicator->InputHP(totalHP, currentHP);
}

void Scorpion::updateTerrain() {
	if (!inFrustum)
		return;

	if (currentTerrain) {
		terrainUtil.InputPosition(position);
		terrainUtil.ClampToTerrain(currentTerrain->GetTerrain(), position, 0.0);
		hpIndicator->SetRenderState(inFrustum);
	}
}

void Scorpion::updateDetectPlayer() {
	size_t size = scene.LayerSize(LAYER_PLAYER);
	for (int i = 0; i < size; i++) {
		if (auto player = scene.FindMulti("player", LAYER_PLAYER, i); player) {
			auto playerOOBB = player->GetOOBB();
			if (lookRange.CheckCollision(playerOOBB)) {
				XMFLOAT3 playerPosition = player->GetPosition();
				playerPosition.y += player->GetSize().y * 1.5;
				Ray newRay = Math::CalcRayVector(position, playerPosition);
				
				bool isBlocked{};
				for (auto& B : mapBounds) {
					if (Math::CheckRayCollision(newRay, B)) {
						currentState = SCOR_IDLE;
						isBlocked = true;
						break;
					}
				}

				if (!isBlocked) {
					rotationDest = Math::CalcDegree3D(position, playerPosition);

					// 공격 범위에 플레이어 바운드가 닿으면 공격 상태 활성화
					if (attackBound.CheckCollision(playerOOBB))
						currentState = SCOR_ATTACK;

					// 아니라면 추격 상태로 전환
					else {
						Math::Normalize2DAngleTo360(rotationDest.y);
						currentState = SCOR_WALK;
					}
				}
			}

			else
				currentState = SCOR_IDLE;
		}
	}
}

void Scorpion::updateState() {
	if (prevState != currentState) {
		switch (currentState) {
		case SCOR_IDLE:
			scorpionFBX.SelectAnimation("Idle");
			scorpionFBX.SetSpeed(1.0);
			break;

		case SCOR_WALK:
			scorpionFBX.SelectAnimation("Walk");
			scorpionFBX.SetSpeed(4.0);
			break;

		case SCOR_ATTACK:
			scorpionFBX.SelectAnimation("Attack 1");
			scorpionFBX.SetSpeed(2.0);
			break;

		case SCOR_DEATH:
			scorpionFBX.SelectAnimation("Death");
			scorpionFBX.SetSpeed(1.0);
			break;
		}
		SendMonstertypePacket(2, currentState,ID);
		prevState = currentState;
	}
}

void Scorpion::updateAnimation(float Delta) {
	if (currentState == SCOR_WALK)
		scorpionFBX.UpdateAnimation(Delta, true, !inFrustum);
	else
		scorpionFBX.UpdateAnimation(Delta, false, !inFrustum);
}

void Scorpion::updateMove(float Delta) {
	rotation.y = Math::LerpDegrees(rotation.y, rotationDest.y, 15.0 * Delta);
	if (currentState == SCOR_WALK) 
		Math::MoveWithSlide(position, rotation.y, 6.0, 0.0, scorBound, mapBounds, Delta);
}

void Scorpion::Update(float Delta) {
	updateBound(Delta);
	updateTerrain();
	updateIndicator();
	updateState();
	updateAnimation(Delta);
	updateDetectPlayer();
	updateMove(Delta);
}

void Scorpion::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	if (currentState == SCOR_WALK)
		Transform::Move(ScaleMatrix, -scorpionFBX.GetInplaceDelta());
	RenderFBX(scorpionFBX, TEX.scorpion);

	hitBox[0].UpdateAnimated(scorpionFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 8);
	hitBox[1].UpdateAnimated(scorpionFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 9);
	hitBox[2].UpdateAnimated(scorpionFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 12);

	for (int i = 0; i < 3; i++)
		hitBox[i].Render();

	frustumAABB.Render();
}
