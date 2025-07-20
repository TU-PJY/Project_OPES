#include "PlantMonster.h"
#include "HP_Indicator.h"
#include "MathUtil.h"
#include "PickingUtil.h"
#include "PoisonBall.h"

// 히트박스 업데이트
void PlantMonster::updateHitBox(float Delta) {
	if (currentState == PLANT_DEATH)
		return;

	// 디펜스 모드일때만 실시간으로 프러스텀 바운드를 업데이트 한다.
	if(defenseModeState)
		frustumBound.Update(position, 10.0);

	for (int i = 0; i < 3; i++)
		hitBox[i].UpdateDelta(Delta);
}

// 공격 대상 감지 진행
void PlantMonster::updateTargetDetect() {
	if (currentState == PLANT_DEATH)
		return;

	// 디펜스 모드 시에는 중앙 건물만을 공격하므로 플레이어 인식 안 함
	if (defenseModeState)
		return;

	size_t layerSize = scene.LayerSize(LAYER_PLAYER);
	for (int i = 0; i < layerSize; i++) {
		if (auto player = scene.FindMulti("player", LAYER_PLAYER, i); player) {
			// 시야 범위 내에서 플레이어가 감지되는지 확인
			if (lookRange.CheckCollision(player->GetOOBB())) {

				// 플레이어가 감지되면 자신과 플레이어 사이의 광선벡터 계산
				XMFLOAT3 playerPosition = player->GetPosition();
				playerPosition.y += player->GetSize().y * 1.5;
				Ray rayVector = Math::CalcRayVector(position, playerPosition);
				bool isBlocking{};

				// 광선이 맵 바운드박스에 충돌하면 IDLE 유지, 그렇지 않다면 ATTACK으로 상태 변경
				for (auto& B : mapBoundData) {
					if (Math::CheckRayCollision(rayVector, B)) {
						currentState = PLANT_IDLE;
						isBlocking = true;
						break;
					}
				}

				if (!isBlocking) {
					currentState = PLANT_ATTACK;
					// 플레이어 방향의 각도로 목표 각도 설정
					destRotation = Math::CalcDegree3D(position, playerPosition);
					Math::Normalize2DAngleTo360(destRotation.y);
					targetPosition = playerPosition;
				}
			}

			// 플레이어가 시야 밖으로 나가면 다시 IDLE로 상태 변경
			else
				currentState = PLANT_IDLE;
		}
	}
}

// 공격 타이밍 업데이트
// 일정 시간마다 구체를 발사한다.
void PlantMonster::updateAttack(float Delta) {
	if (currentState != PLANT_ATTACK) {
		shootState = false;
		return;
	}

	// 공격 모션에 맞추어 독 구체를 발사한다.
	// 공격 대상을 향해 발사한다.
	// 구체 발사 후 애니메이션의 같은 구간에 도달할 때까지 발사하지 않는다.
	if (plantFBX.GetTimeSectionPassed(plantFBX.GetCurrentAnimationTime() - 0.65f)) {
		if (!shootState) {
			XMFLOAT3 createPosition = Math::CalcForwardOffset(position, rotation.y, 2.0f, size.y * 0.9);
			scene.AddObject(new PoisonBall(createPosition, targetPosition, currentMapName, true), "poisonBall", LAYER3);
			shootState = true;
		}
	}
	else
		shootState = false;

	// 목표 각도로 회전한다.
	rotation.y = Math::LerpDegrees(rotation.y, destRotation.y, 15.0 * Delta);
}

// hp 표시기 업데이트 진행
void PlantMonster::updateIndicatorHP() {
	if (currentState == PLANT_DEATH)
		return;

	if (hpIndicator) {
		hpIndicator->InputPosition(position, size.y * 1.5);
		hpIndicator->InputHP(totalHP, currentHP);
		hpIndicator->SetRenderState(inFrustum);
	}
}

// 디펜스 모드 시 땅에서 올라오는 애니메이션 업데이트
void PlantMonster::updateLiftFromGround(float Delta) {
	if (currentState != PLANT_LIFT)
		return;

	position.y += Delta * 10.0;
	if (position.y > terrainHeight) {
		position.y = terrainHeight;
		behaviorEnabledState = true;
		currentState = PLANT_ATTACK;
	}
}

// 애니메이션 업데이트 진행
// 이전 상태와 현재 상태가 다를 때마다 애니메이션을 변경한다.
void PlantMonster::updateAnimation(float Delta) {
	if (currentState != prevState) {
		switch (currentState) {
		case PLANT_IDLE:
			plantFBX.SelectAnimation("AttackIdle"); break;
		case PLANT_ATTACK:
			plantFBX.SelectAnimation("Attack01"); break;
		case PLANT_DEATH:
			plantFBX.SelectAnimation("Death"); break;
		case PLANT_LIFT:
			plantFBX.SelectAnimation("Magic01charge"); break;
		}

		prevState = currentState;
	}

	// 프러스텀 바깥일 때는 애니메이션 적용 없이 재생 시간만 계산
	plantFBX.UpdateAnimation(Delta, false, !inFrustum);
}

// 죽음 상태 업데이트 진행
void PlantMonster::updateDeleteDelay(float Delta) {
	if (currentState != PLANT_DEATH)
		return;

	deleteDelayTime += Delta;
	if (deleteDelayTime >= 2.3) 
		scene.DeleteObject(this);
}


////////////////////////////////////////////////////////////


// createPosition: 처음 생성될 때 스폰되는 위치
// terrainName: 현재 맵의 터레인 객체 이름
// appearFromGround: 활성화 시 땅 속에서 나옴
PlantMonster::PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, unsigned int ID, bool appearFromGround) {
	// 원본에서 인스턴스 복사
	plantFBX.SelectFBXMesh(MESH.plantMonster);

	// 땅에서 나오는 상태일 경우 디펜스 업데이트 모드 활성화
	tempPosition = createPosition;
	behaviorEnabledState = !appearFromGround;
	defenseModeState = appearFromGround;

	// 땅에서 나오는 상태일 경우 별도의 상태를 지정한다.
	if (behaviorEnabledState)
		currentState = PLANT_IDLE;
	else
		currentState = PLANT_LIFT;

	TerrainUtil terrainUtil;

	// 고정형 몬스터이므로 생성 이후로는 터레인 업데이트를 진행하지 않는다.
	if (auto terrain = scene.Find(terrainName); terrain) {
		terrainUtil.InputPosition(tempPosition);
		terrainUtil.ClampToTerrain(terrain->GetTerrain(), tempPosition, 0.0);
		position = tempPosition;

		// 현재 위치에서의 터레인 높이 구하기
		terrainHeight = tempPosition.y;

		//  땅에서 나오는 상태일 경우 높이를 땅 속으로 옮긴다.
		if (!behaviorEnabledState)
			position.y -= 10.0;

		mapBoundData = terrain->GetMapWallOOBB();

		currentMapName = terrainName;
	}
	else 
		position = tempPosition;

	// 디펜스 모드 일때는 중앙 건물 만을 공격하므로 생성 이후로는 회전각도 업데이트를 하지 않는다.
	if (defenseModeState) {
		if (auto centerBuilding = scene.Find("center_building"); centerBuilding) {
			XMFLOAT3 centerBuildingPosition = centerBuilding->GetPosition();
			destRotation.y = Math::CalcDegree2D(position.z, position.x, centerBuildingPosition.z, centerBuildingPosition.x);
			targetPosition = centerBuildingPosition;
		}
	}

	// 자기 소유의 hp 표시기 객체 추가
	hpIndicator = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER2);

	for (int i = 0; i < 3; i++)
		hitBox[i].SetUpdateFrequency(24);

	// 어드벤처 모드에서만 시야를 설정한다.
	if (!defenseModeState)
		lookRange.Update(XMFLOAT3(position.x, position.y + size.y, position.z), 80.0);

	// 프러스텀 바운드 설정
	frustumBound.Update(position, 10.0);

	this->ID = ID;
}

PlantMonster::~PlantMonster() {
	if (hpIndicator) 
		scene.DeleteObject(hpIndicator);
}

// 모든 업데이트
void PlantMonster::Update(float Delta) {
	// 프러스텀 검사
	inFrustum = camera.CheckFrustum(frustumBound);

	updateHitBox(Delta);
	updateAnimation(Delta);

	if (behaviorEnabledState) {
		updateTargetDetect();
		updateAttack(Delta);
		updateDeleteDelay(Delta);
	}

	updateIndicatorHP();
	updateLiftFromGround(Delta);
}

// 렌더링
void PlantMonster::Render() {
	// 프러스텀에 들어가있지 않다면 렌더링을 건너뛴다.
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	RenderFBX(plantFBX, TEX.plantMonster);
	UpdatePickMatrix();

	hitBox[0].UpdateAnimated(plantFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 0);
	hitBox[1].UpdateAnimated(plantFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 2);
	hitBox[2].UpdateAnimated(plantFBX, TranslateMatrix, RotateMatrix, ScaleMatrix, 4);

	for (int i = 0; i < 3; i++)
		hitBox[i].Render();

	//lookRange.Render();
}

bool PlantMonster::CheckHit(BoundSphere& Sphere, int damage) {
	if (currentState == PLANT_DEATH)
		return false;

	bool hit{};
	for (int i = 0; i < 3; i++) {
		if (hitBox[i].CheckCollision(Sphere)) {
			hit = true;
			break;
		}
	}

	if (hit) {
		currentHP -= damage;
		if (currentHP <= 0) {
			currentState = PLANT_DEATH;
			if (hpIndicator) {
				scene.DeleteObject(hpIndicator);
				hpIndicator = nullptr;
			}

			// 디펜스 모드일 경우 남은 적 카운트를 감소시킨다.
			if (defenseModeState) {
				GLOBAL.map1DefenseEnemyRemained--;
				if (GLOBAL.map1DefenseEnemyRemained == 0)
					GLOBAL.map1DefenseState = false;
			}
		}
		return true;
	}

	return false;
}