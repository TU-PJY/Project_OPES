#include "Scorpion.h"
#include "HP_Indicator.h"
#include "CameraUtil.h"
#include "PickingUtil.h"

void SendMonstertypePacket(unsigned int monsterType, unsigned int monsterState, unsigned int id);
void SendMonsterMovePacket(float x, float y, float z, float angle, unsigned int monsterId, unsigned int targetid);

Scorpion::Scorpion(const XMFLOAT3& createPosition, unsigned int ID) {
	position = createPosition;
	positionDest = createPosition;
	hpIndicator = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER3);

	for(int i = 0; i < 3; i++)
		hitBox[i].SetUpdateFrequency(SCORPION_DAMAGE);

	this->ID = ID;
}

Scorpion::~Scorpion() {
	if (hpIndicator)
		scene.DeleteObject(hpIndicator);
}

// 서버로부터 위치를 입력 받으면 바로 반영하지 않고 1프레임 늦게 반영한다.
void Scorpion::updateInputedPosition() {
	/*if (!positionInputedState)
		return;

	position = inputedPosition;
	positionInputedState = false;*/
}

void Scorpion::updateBound(float Delta) {
	if (currentState == SCOR_DEATH)
		return;

	frustumAABB.Update(XMFLOAT3(position.x, position.y + size.y, position.z), XMFLOAT3(size.x * 2.0, size.y * 0.8, size.z * 2.0));
	inFrustum = camera.CheckFrustum(frustumAABB);

	for (int i = 0; i < 3; i++)
		hitBox[i].UpdateDelta(Delta);

	lookRange.Update(position, 70.0);
	scorBound.Update(XMFLOAT3(position.x, position.y + 0.5, position.z), 1.0);
	XMFLOAT3 attackBoundPosition = Math::CalcForwardOffset(position, rotation.y, 4.0, size.y * 0.5);
	attackBound.Update(attackBoundPosition, 3.0);
}

void Scorpion::updateIndicator() {
	if (currentState == SCOR_DEATH)
		return;

	if (hpIndicator) {
		hpIndicator->InputPosition(position, size.y * 1.7);
		hpIndicator->InputHP(totalHP, currentHP);
		hpIndicator->SetRenderState(inFrustum);
	}
}

void Scorpion::updateTerrain() {
	if (!inFrustum)
		return;

	terrainUtil.InputPosition(position);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.0);
}

void Scorpion::sendCurrentState() {
	if (serverState == currentState)
		return;

	serverState = currentState;
	SendMonstertypePacket(2, currentState, ID);
}

void Scorpion::sendCurrentPosition() {
	if (prevTargetID == currentTargetID)
		return;

	SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
	prevTargetID = currentTargetID;
}

void Scorpion::updateDetectPlayer(float Delta) {
	if (currentState == SCOR_DEATH)
		return;

	// 일정 간격마다 전송 활성화
	sendState = false;
	sendDelay += Delta;
	if (sendDelay >= destDelay) {
		sendDelay -= destDelay;
		sendState = true;
	}

	size_t size = scene.LayerSize(LAYER_PLAYER);

	// 현재 아무도 추격 안 하거나 나를 추격 중이면 나를 추적하도록 한다.
	if (currentTargetID == GLOBAL.myID || currentTargetID == 0) {
		for (int i = 0; i < size; i++) {
			if (auto player = scene.FindMulti("player", LAYER_PLAYER, i); player) {
				auto playerOOBB = player->GetOOBB();
				if (lookRange.CheckCollision(playerOOBB)) {
					XMFLOAT3 playerPosition = player->GetPosition();
					playerPosition.y += player->GetSize().y * 1.5;
					Ray newRay = Math::CalcRayVector(position, playerPosition);

					bool isBlocked{};
					for (auto& B : GLOBAL.mapOOBBdata) {
						if (Math::CheckRayCollision(newRay, B)) {
							currentState = SCOR_IDLE; 
							currentTargetID = 0;
							isBlocked = true;

							SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
							SendMonstertypePacket(2, currentState, ID);
							
							break;
						}
					}

					if (!isBlocked) {
						rotationDest = Math::CalcDegree3D(position, playerPosition);

						// 공격 범위에 플레이어 바운드가 닿으면 공격 상태 활성화
						if (attackBound.CheckCollision(playerOOBB)) {
							currentState = SCOR_ATTACK;
							currentTargetID = GLOBAL.myID;

							if (sendState) {
								SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
								SendMonstertypePacket(2, currentState, ID);
							}
						}

						// 아니라면 추격 상태로 전환
						else {
							Math::Normalize2DAngleTo360(rotationDest.y);
							currentState = SCOR_WALK;
							currentTargetID = GLOBAL.myID;

							if (sendState) {
								SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
								SendMonstertypePacket(2, currentState, ID);
							}
						}
					}
				}

				else {
					currentState = SCOR_IDLE;
					currentTargetID = 0;
				}
			}
		}
	}

	if (serverState != currentState) {
		SendMonstertypePacket(2, currentState, ID);
		serverState = currentState;
	}

	if (prevTargetID != currentTargetID) {
		SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
		prevTargetID = currentTargetID;
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
	if (currentState == SCOR_DEATH)
		return;

	rotation.y = Math::LerpDegrees(rotation.y, rotationDest.y, 15.0 * Delta);

	// 나를 추격하는 상태일때만 MoveWithSlide를 실행한다.
	if (currentState == SCOR_WALK && currentTargetID == GLOBAL.myID)
		Math::MoveWithSlide(positionDest, rotation.y, 5.0, 0.0, scorBound, GLOBAL.mapOOBBdata, Delta);
		
//	Math::LerpXMFLOAT3(position, positionDest, 10.0, Delta);
	position.x = std::lerp(position.x, positionDest.x, 10.0 * Delta);
	position.z = std::lerp(position.z, positionDest.z, 10.0 * Delta);
}

void Scorpion::updateDeath() {
	if (currentState != SCOR_DEATH)
		return;

	if (scorpionFBX.GetAnimationEndState())
		scene.DeleteObject(this);
}

void Scorpion::updateAttack() {
	if (currentState != SCOR_ATTACK) {
		attackDid = false;
		return;
	}

	if (scorpionFBX.GetTimeSectionPassed(40.3667 - 0.7)) {
		if (!attackDid) {
			if (currentTargetID == GLOBAL.myID) {
				if (auto player = scene.SearchLayer(LAYER_PLAYER, "player"); player) {
					player->GiveDamage(20);
					std::cout << "scorpion attack" << std::endl;
				}
			}
			attackDid = true;
		}
	}
	else 
		attackDid = false;
}

void Scorpion::Update(float Delta) {
	updateInputedPosition();
	updateBound(Delta);
	updateIndicator();
	updateState();
	updateAnimation(Delta);
	updateDetectPlayer(Delta);
	updateMove(Delta);
	updateTerrain();
	updateAttack();
	updateDeath();
}

void Scorpion::Render() {
	if (!inFrustum)
		return;

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

bool Scorpion::CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) {
	for (int i = 0; i < 3; i++) {
		if (hitBox[i].oobb.Intersects(start, direction, distance))
			return true;
	}

	return false;
}

bool Scorpion::CheckHit(float& distance) {
	if (currentState == SCOR_DEATH)
		return false;

	for (int i = 0; i < 3; i++) {
		if (PickingUtil::PickByViewportOOBB(XMFLOAT2(0.0, 0.0), distance, hitBox[i]))
			return true;
	}

	return false;
}

XMFLOAT3 Scorpion::GetPosition() {
	return position;
}

bool Scorpion::CheckHit(BoundSphere& bound) {
	if (currentState == SCOR_DEATH)
		return false;

	for (int i = 0; i < 3; i++) {
		if (hitBox[i].CheckCollision(bound))
			return true;
	}

	return false;
}

void Scorpion::GiveDamage(int damage) {
	if (currentState == SCOR_DEATH)
		return;

	currentHP -= damage;

	if (currentHP <= 0) {
		if (hpIndicator) {
			scene.DeleteObject(hpIndicator);
			hpIndicator = nullptr;
		}
		currentState = SCOR_DEATH;
		SendMonstertypePacket(2, currentState, ID);
	}
}

void Scorpion::InputHP(int currentHP) {
	if (currentState == SCOR_DEATH)
		return;

	this->currentHP = currentHP;

	if (this->currentHP == 0) {
		if (hpIndicator) {
			scene.DeleteObject(hpIndicator);
			hpIndicator = nullptr;
		}
		currentState = SCOR_DEATH;
		//SendMonstertypePacket(2, currentState, ID);
	}
}

unsigned int Scorpion::GetID() {
	return ID;
}

void Scorpion::InputState(unsigned int state) {
	//if (currentTargetID == GLOBAL.myID)
		//return;
	if (currentState == SCOR_DEATH)
		return;

	currentState = state;
	if (currentState == SCOR_DEATH) {
		currentHP = 0;
		if (hpIndicator) {
			scene.DeleteObject(hpIndicator);
			hpIndicator = nullptr;
		}
		//SendMonstertypePacket(2, currentState, ID);
	}
}

void Scorpion::InputPosition(XMFLOAT3& position) {
	//if (currentTargetID == GLOBAL.myID)
		//return;

	positionDest = position;
}

void Scorpion::InputRotation(float degrees) {
	//if (currentTargetID == GLOBAL.myID)
	//	return;

	rotationDest.y = degrees;
}

void Scorpion::InputTargetID(unsigned int target) {
	//if (currentTargetID == GLOBAL.myID)
		//return;

	currentTargetID = target;
}

bool Scorpion::GetDeathState() {
	if (currentState == SCOR_DEATH)
		return true;
	return false;
}

int Scorpion::GetHP()
{
	return currentHP;
}
