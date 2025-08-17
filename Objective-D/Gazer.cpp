#include "Gazer.h"
#include "CameraUtil.h"
#include "HP_Indicator.h"
#include "PickingUtil.h"

void SendMonstertypePacket(unsigned int monsterType, unsigned int monsterState, unsigned int id);
void SendMonsterMovePacket(float x, float y, float z, float angle, unsigned int monsterId, unsigned int targetid);
void SendCenterBuildingPacket(int hp);
void SendPtoMDamagePacket(unsigned int monsterID, int attackHp);

Gazer::Gazer(const xmfloat3& createPosition, unsigned int ID, bool defenseState) {
	position = createPosition;
	positionDest = createPosition;
	hitBox.SetUpdateFrequency(24);
	position.y = -1.8;
	this->defenseState = defenseState;
	this->ID = ID;

	if (defenseState)
		position.y = -15.0;

	hpInd = scene.AddObject(new HP_Indicator, "ind", LAYER3);
}

Gazer::~Gazer() {
	if (hpInd)
		scene.DeleteObject(hpInd);

	GLOBAL.Map3DefenseEnemyRemained--;
	if (GLOBAL.Map3DefenseEnemyRemained == 0)
		GLOBAL.Map3DefenseState = false;
}

void Gazer::gz_updateIndicator() {
	if (hpInd) {
		hpInd->InputHP(totalHP, currentHP);
		hpInd->InputPosition(position, 10.0);
	}
}

void Gazer::gz_updateLift(float Delta) {
	if (!defenseState)
		return;

	if (!behaviorEnabled) {
		position.y += Delta * 10.0;
		if (position.y >= -1.8) {
			position.y = -1.8;
			behaviorEnabled = true;
		}
	}
}

void Gazer::gz_updateBound() {
	xmfloat3 boundPosition = xmfloat3(position.x, position.y + size.y * 2.0, position.z);
	frustumBound.Update(boundPosition, 10.0);
	inFrustum = camera.CheckFrustum(frustumBound);

	xmfloat3 boundPos = xmfloat3(position.x, position.y + size.y, position.z);
	boundPos = Math::CalcForwardOffset(boundPos, rotation.y, 6.0, 0.0);
	attackBound.Update(boundPos, xmfloat3(2.0, 2.0, 4.0), rotation);
	lookRange.Update(boundPosition, 100.0);
}

void Gazer::gz_updateAnimation(float Delta) {
	if(currentState == GAZER_WALK)
		gazerFBX.UpdateAnimation(Delta, true, !inFrustum);
	else
		gazerFBX.UpdateAnimation(Delta, false, !inFrustum);

	hitBox.UpdateDelta(Delta);
}

void Gazer::gz_updateAttack() {
	if (currentState != GAZER_ATTACK) {
		attackDid = false;
		return;
	}

	if (gazerFBX.GetTimeSectionPassed(63.6)) {
		if (!attackDid) {
			if (!defenseState) {
				if (currentTargetID == GLOBAL.myID) {
					if (auto player = scene.SearchLayer(LAYER_PLAYER, "player"); player) {
						player->GiveDamage(GAZER_DAMAGE);
						xmfloat3 rotation = Math::CalcDegree3D(position, player->GetPosition());
						player->GiveKnockback(rotation.y, 150.0);
					}
				}
			}
			else {
				if (auto center = scene.SearchLayer(LAYER1, "center_building"); center) {
					//if (!GLOBAL.useServer)
						center->GiveDamage(10);
					//else
						//SendCenterBuildingPacket(10);
				}
			}
			attackDid = true;
		}
	}
	else
		attackDid = false;
}

void Gazer::gz_updateTerrainCollision() {
	if (currentState != GAZER_DEATH)
		return;

	Ray playerRay = Math::CalcRayVector(xmfloat3(position.x, position.y + 40.0, position.z), xmfloat3(position.x, position.y - 40.0, position.z));
	float Distance;
	xmfloat3 newPosition = terrainUtil.CheckCollisionRay(GLOBAL.mapTerrain, playerRay.Origin, playerRay.Direction, Distance);
	if (newPosition.x == 0.0 && newPosition.y == 0.0 && newPosition.z == 0.0 || newPosition.y <= -5.0)
		fallDown = true;
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

void Gazer::gz_detectPlayer(float Delta) {
	if (currentState == GAZER_DEATH)
		return;

	if (behaviorEnabled) {
		if (auto center = scene.SearchLayer(LAYER1, "center_building"); center) {
			rotationDest = Math::CalcDegree3D(position, center->GetPosition());
			if (attackBound.CheckCollision(center->GetOOBB())) {
				currentState = GAZER_ATTACK;
			}
			else
				currentState = GAZER_WALK;
		}
	}

	// 디펜스용 스폰 시 감지 안 함
	if (defenseState)
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
		if (auto player = scene.SearchLayer(LAYER_PLAYER, "player"); player) {
			auto playerOOBB = player->GetOOBB();
			if (lookRange.CheckCollision(playerOOBB)) {
				XMFLOAT3 playerPosition = player->GetPosition();
				playerPosition.y += player->GetSize().y * 1.5;
				Ray newRay = Math::CalcRayVector(position, playerPosition);

				bool isBlocked{};
				for (auto& B : GLOBAL.mapOOBBdata) {
					if (Math::CheckRayCollision(newRay, B)) {
						currentState = GAZER_IDLE;
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
						currentState = GAZER_ATTACK;
						currentTargetID = GLOBAL.myID;

						if (sendState) {
							SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
							SendMonstertypePacket(2, currentState, ID);
						}
					}

					// 아니라면 추격 상태로 전환
					else {
						Math::Normalize2DAngleTo360(rotationDest.y);
						currentState = GAZER_WALK;
						currentTargetID = GLOBAL.myID;

						if (sendState) {
							SendMonsterMovePacket(position.x, position.y, position.z, rotation.y, ID, currentTargetID);
							SendMonstertypePacket(2, currentState, ID);
						}
					}
				}
			}

			else {
				currentState = GAZER_IDLE;
				currentTargetID = 0;
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

void Gazer::gz_updateMove(float Delta) {
	if (currentState == GAZER_DEATH)
		return;

	rotation.y = Math::LerpDegrees(rotation.y, rotationDest.y, 15.0 * Delta);
	
	// 나를 추격하는 상태일때만 MoveWithSlide를 실행한다.
	if (!defenseState) {
		if (currentState == GAZER_WALK && currentTargetID == GLOBAL.myID)
			Math::MoveWithSlide(positionDest, rotation.y, 6.0, 0.0, gazerBound, GLOBAL.mapOOBBdata, Delta);
	}

	else {
		if (currentState == GAZER_WALK) {
			Math::MoveForward(position, rotation.y, 6.0 * Delta);
		//	Math::MoveStrafe(position, rotation.y, 6.0 * Delta);
		}
	}

	//	Math::LerpXMFLOAT3(position, positionDest, 10.0, Delta);
	if (!defenseState) {
		position.x = std::lerp(position.x, positionDest.x, 10.0 * Delta);
		position.z = std::lerp(position.z, positionDest.z, 10.0 * Delta);
	}

	if (fallDown) {
		if (fallDown) {
			fallAcc += Delta * 0.5;
			heightOffset -= fallAcc;
		}
	}
}

void Gazer::gz_updateDeath() {
	if (currentState != GAZER_DEATH)
		return;

	if (gazerFBX.GetTimeSectionPassed(86.9))
		position.y = -0.4;

	if (gazerFBX.GetAnimationEndState())
		scene.DeleteObject(this);
}

void Gazer::Update(float Delta) {
	gz_updateAnimation(Delta);
	gz_updateState();
	gz_updateLift(Delta);
	gz_updateTerrainCollision();
	gz_updateBound();
	gz_detectPlayer(Delta);
	gz_updateMove(Delta);
	gz_updateIndicator();
	gz_updateAttack();
	gz_updateDeath();
}

void Gazer::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position.x, position.y + heightOffset, position.z);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	if(currentState == GAZER_WALK)
		Transform::Move(ScaleMatrix, 0.0, 0.0, -gazerFBX.GetInplaceDelta().z);
	RenderFBX(gazerFBX, TEX.gazer);
	hitBox.UpdateAnimated(gazerFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 4);
	//frustumBound.Render();
	//hitBox.Render();
	//attackBound.Render();
	//lookRange.Render();
}

OOBB Gazer::GetOOBB() {
	return hitBox;
}

bool Gazer::CheckHit(BoundSphere& sphere) {
	if (hitBox.CheckCollision(sphere))
		return true;
	return false;
}

bool Gazer::CheckHit(float& distance) {
	if (currentState == GAZER_DEATH)
		return false;

	if(PickingUtil::PickByViewportOOBB(xmfloat2(0.0, 0.0), distance, hitBox))
		return true;
	return false;
}

xmfloat3 Gazer::GetPosition() {
	xmfloat3 retval(position.x, position.y + 4.0, position.z);
	return retval;
}

void Gazer::GiveDamage(int damage) {
	if (currentState == GAZER_DEATH)
		return;

	if (!GLOBAL.useServer) {
		currentHP -= damage;
		if (currentHP <= 0) {
			currentHP = 0;
			currentState = GAZER_DEATH;
			if (hpInd) {
				scene.DeleteObject(hpInd);
				hpInd = nullptr;
			}
		}
	}
	else
		SendPtoMDamagePacket(ID, damage);
	// packet
}

void Gazer::InputState(unsigned int State) {
	if (currentState == GAZER_DEATH)
		return;

	currentState = State;
	if (currentState == GAZER_DEATH) {
		currentHP = 0;
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
	}
}

void Gazer::InputPosition(XMFLOAT3& Position) {
	positionDest = Position;
}

void Gazer::InputRotation(float Rotation) {
	rotationDest.y = Rotation;
}

void Gazer::InputTargetID(unsigned int ID) {
	currentTargetID = ID;
}

void Gazer::InputHP(int hp) {
	if (currentState == GAZER_DEATH)
		return;

	currentHP = hp;
	if (currentHP <= 0) {
		currentHP = 0;
		currentState = GAZER_DEATH;
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
	}
}

bool Gazer::GetDeathState() {
	if (currentState == GAZER_DEATH)
		return true;
	return false;
}

bool Gazer::CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) {
	if (currentState == GAZER_DEATH)
		return false;

	if (hitBox.oobb.Intersects(start, direction, distance))
		return true;
	return false;
}

unsigned int Gazer::GetID()
{
	return ID;
}
