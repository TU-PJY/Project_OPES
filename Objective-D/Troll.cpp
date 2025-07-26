#include "Troll.h"
#include "CameraUtil.h"
#include "HP_Indicator.h"
#include "PickingUtil.h"
#include "ClampUtil.h"

void SendMonstertypePacket(unsigned int monsterType, unsigned int monsterState, unsigned int id);
void SendMonsterMovePacket(float x, float y, float z, float angle, unsigned int monsterId, unsigned int targetid);

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
	lookRange.Update(boundPosition, 60.0);
	trollBound.Update(XMFLOAT3(position.x, position.y + 0.5, position.z), 2.0);
}

void Troll::detectPlayer(float Delta) {
	if (currentState == TROLL_DEATH)
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
							currentState = TROLL_IDLE;
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
							currentState = TROLL_ATTACK;
							currentTargetID = GLOBAL.myID;

							if (sendState) {
								SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
								SendMonstertypePacket(2, currentState, ID);
							}
						}

						// 아니라면 추격 상태로 전환
						else {
							Math::Normalize2DAngleTo360(rotationDest.y);
							currentState = TROLL_MOVE;
							currentTargetID = GLOBAL.myID;

							if (sendState) {
								SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
								SendMonstertypePacket(2, currentState, ID);
							}
						}
					}
				}

				else {
					currentState = TROLL_IDLE;
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

void Troll::updateMove(float Delta) {
	if (currentState == TROLL_DEATH)
		return;

	rotation.y = Math::LerpDegrees(rotation.y, rotationDest.y, 15.0 * Delta);

	// 나를 추격하는 상태일때만 MoveWithSlide를 실행한다.
	if (currentState == TROLL_MOVE && currentTargetID == GLOBAL.myID)
		Math::MoveWithSlide(positionDest, rotation.y, 6.0, 0.0, trollBound, GLOBAL.mapOOBBdata, Delta);

	//	Math::LerpXMFLOAT3(position, positionDest, 10.0, Delta);
	position.x = std::lerp(position.x, positionDest.x, 10.0 * Delta);
	position.z = std::lerp(position.z, positionDest.z, 10.0 * Delta);
}

void Troll::updateAttack() {
	if (currentState != TROLL_ATTACK) {
		attackDid = false;
		return;
	}

	if (trollFBX.GetTimeSectionPassed(25.0)) {
		if (!attackDid) {
			if (currentTargetID == GLOBAL.myID) {
				if (auto player = scene.SearchLayer(LAYER_PLAYER, "player"); player) {
					player->GiveDamage(30);
					std::cout << "troll attack" << std::endl;
				}
			}
			attackDid = true;
		}
	}
	else
		attackDid = false;
}

void Troll::updateDeath() {
	if (currentState != TROLL_DEATH)
		return;

	if (trollFBX.GetAnimationEndState())
		scene.DeleteObject(this);
}

void Troll::Update(float Delta) {
	detectPlayer(Delta);
	updateMove(Delta);
	updateTerrainCollision();
	updateIndicator();
	updateBound();
	updateState();
	updateAttack();
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

XMFLOAT3 Troll::GetPosition() {
	return position;
}

bool Troll::CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) {
	if (currentState == TROLL_DEATH)
		return false; 

	if (trollOOBB.oobb.Intersects(start, direction, distance))
		return true;
	return false;
}

bool Troll::CheckHit(float& distance) {
	if (currentState == TROLL_DEATH)
		return false;

	if (PickingUtil::PickByViewportOOBB(xmfloat2(0.0, 0.0), distance, trollOOBB))
		return true;
	return false;
}

bool Troll::CheckHit(BoundSphere& bound) {
	if (currentState == TROLL_DEATH)
		return false;

	if (trollOOBB.CheckCollision(bound))
		return true;
	return false;
}

void Troll::GiveDamage(int damage) {
	if (currentState == TROLL_DEATH)
		return;

	currentHP -= damage;
	Clamp::LimitValue(currentHP, 0, CLAMP_DIR_LESS);
	if (currentHP == 0)
		currentState = TROLL_DEATH;
}

void Troll::InputHP(int currentHP) {
	if (currentState == TROLL_DEATH)
		return;

	this->currentHP = currentHP;

	if (this->currentHP == 0) {
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
		currentState = TROLL_DEATH;
		//SendMonstertypePacket(2, currentState, ID);
	}
}

unsigned int Troll::GetID() {
	return ID;
}

void Troll::InputState(unsigned int state) {
	if (currentState == TROLL_DEATH)
		return;

	currentState = state;
	if (currentState == TROLL_DEATH) {
		currentHP = 0;
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
		//SendMonstertypePacket(2, currentState, ID);
	}
}

void Troll::InputPosition(XMFLOAT3& position) {
	positionDest = position;
}

void Troll::InputRotation(float degrees) {
	rotationDest.y = degrees;
}

void Troll::InputTargetID(unsigned int target) {
	currentTargetID = target;
}

bool Troll::GetDeathState() {
	if (currentState == TROLL_DEATH)
		return true;
	return false;
}

int Troll::GetHP() {
	return currentHP;
}
