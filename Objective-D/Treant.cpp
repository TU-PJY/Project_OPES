#include "Treant.h"
#include "CameraUtil.h"
#include "HP_Indicator.h"
#include "PickingUtil.h"
#include "ClampUtil.h"

void SendMonstertypePacket(unsigned int monsterType, unsigned int monsterState, unsigned int id);
void SendMonsterMovePacket(float x, float y, float z, float angle, unsigned int monsterId, unsigned int targetid);

Treant::Treant(const xmfloat3& createPosition, unsigned int ID, bool defenseModeState) {
	position = createPosition;
	positionDest = createPosition;
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
		hpInd->InputPosition(position, 8.0);
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
	lookRange.Update(boundPosition, 60.0);
	treantBound.Update(XMFLOAT3(position.x, position.y + 0.5, position.z), 2.0);

	XMFLOAT3 attackBoundPosition = Math::CalcForwardOffset(position, rotation.y, 5.0, size.y * 0.5);
	attackBound.Update(attackBoundPosition, 5.0);
}

void Treant::detectPlayer(float Delta) {
	if (currentState == TREANT_DEATH)
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
							currentState = TREANT_IDLE;
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
							currentState = TREANT_ATTACK;
							currentTargetID = GLOBAL.myID;

							if (sendState) {
								SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
								SendMonstertypePacket(2, currentState, ID);
							}
						}

						// 아니라면 추격 상태로 전환
						else {
							Math::Normalize2DAngleTo360(rotationDest.y);
							currentState = TREANT_MOVE;
							currentTargetID = GLOBAL.myID;

							if (sendState) {
								SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
								SendMonstertypePacket(2, currentState, ID);
							}
						}
					}
				}

				else {
					currentState = TREANT_IDLE;
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

void Treant::updateMove(float Delta) {
	if (currentState == TREANT_DEATH)
		return;

	rotation.y = Math::LerpDegrees(rotation.y, rotationDest.y, 15.0 * Delta);

	// 나를 추격하는 상태일때만 MoveWithSlide를 실행한다.
	if (currentState == TREANT_MOVE && currentTargetID == GLOBAL.myID)
		Math::MoveWithSlide(positionDest, rotation.y, 5.0, 0.0, treantBound, GLOBAL.mapOOBBdata, Delta);

	//	Math::LerpXMFLOAT3(position, positionDest, 10.0, Delta);
	position.x = std::lerp(position.x, positionDest.x, 10.0 * Delta);
	position.z = std::lerp(position.z, positionDest.z, 10.0 * Delta);
}

void Treant::updateAttack() {
	if (currentState != TREANT_ATTACK) {
		attackDid = false;
		return;
	}

	if (treantFBX[TREANT_ATTACK].GetTimeSectionPassed(1.0)) {
		if (!attackDid) {
			if (currentTargetID == GLOBAL.myID) {
				if (auto player = scene.SearchLayer(LAYER_PLAYER, "player"); player) {
					player->GiveDamage(TREANT_DAMAGE);
					std::cout << "treant attack" << std::endl;
				}
			}
			attackDid = true;
		}
	}
	else
		attackDid = false;
}

void Treant::updateDeath() {
	if (currentState != TREANT_DEATH)
		return;

	if (treantFBX[TREANT_DEATH].GetAnimationEndState())
		scene.DeleteObject(this);
}

void Treant::Update(float Delta) {
	detectPlayer(Delta);
	updateMove(Delta);
	updateTerrainCollision();
	updateIndicator();
	updateBound();
	updateState();
	updateAttack();
	updateDeath();
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

XMFLOAT3 Treant::GetPosition() {
	return position;
}

bool Treant::CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) {
	if (currentState == TREANT_DEATH)
		return false;

	if (treantOOBB.oobb.Intersects(start, direction, distance))
		return true;
	return false;
}

bool Treant::CheckHit(float& distance) {
	if (currentState == TREANT_DEATH)
		return false;

	if (PickingUtil::PickByViewportOOBB(xmfloat2(0.0, 0.0), distance, treantOOBB))
		return true;
	return false;
}

bool Treant::CheckHit(BoundSphere& bound) {
	if (currentState == TREANT_DEATH)
		return false;

	if (treantOOBB.CheckCollision(bound))
		return true;
	return false;
}

void Treant::GiveDamage(int damage) {
	if (currentState == TREANT_DEATH)
		return;

	currentHP -= damage;
	Clamp::LimitValue(currentHP, 0, CLAMP_DIR_LESS);
	if (currentHP == 0)
		currentState = TREANT_DEATH;
}

void Treant::InputHP(int currentHP) {
	if (currentState == TREANT_DEATH)
		return;

	this->currentHP = currentHP;

	if (this->currentHP == 0) {
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
		currentState = TREANT_DEATH;
		//SendMonstertypePacket(2, currentState, ID);
	}
}

unsigned int Treant::GetID() {
	return ID;
}

void Treant::InputState(unsigned int state) {
	if (currentState == TREANT_DEATH)
		return;

	currentState = state;
	if (currentState == TREANT_DEATH) {
		currentHP = 0;
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
		//SendMonstertypePacket(2, currentState, ID);
	}
}

void Treant::InputPosition(XMFLOAT3& position) {
	positionDest = position;
}

void Treant::InputRotation(float degrees) {
	rotationDest.y = degrees;
}

void Treant::InputTargetID(unsigned int target) {
	currentTargetID = target;
}

bool Treant::GetDeathState() {
	if (currentState == TREANT_DEATH)
		return true;
	return false;
}

int Treant::GetHP() {
	return currentHP;
}
