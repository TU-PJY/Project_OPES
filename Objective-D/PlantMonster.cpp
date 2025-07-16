#include "PlantMonster.h"
#include "HP_Indicator.h"
#include "MathUtil.h"
#include "PickingUtil.h"

// 히트박스 업데이트
void PlantMonster::updateHitBox() {
	XMFLOAT3 boxPosition = XMFLOAT3(position.x, position.y, position.z);
	XMFLOAT3 boxSize = XMFLOAT3(0.7, size.y * 0.8, 0.7);
	hitBox.Update(boxPosition, boxSize, rotation);
}

// 공격 대상 감지 진행
void PlantMonster::updateTargetDetect() {
	// 디펜스 모드 시에는 중앙 건물만을 공격하므로 플레이어 인식 안 함
	if (defenseModeState)
		return;
}

// hp 표시기 업데이트 진행
void PlantMonster::updateIndicatorHP() {
	if (hpIndicator) {
		hpIndicator->InputPosition(position, size.y * 1.5);
		hpIndicator->InputHP(totalHP, currentHP);
	}
}

// 디펜스 모드 시 땅에서 올라오는 애니메이션 업데이트
void PlantMonster::updateLiftFromGround(float Delta) {
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

	plantFBX.UpdateAnimation(Delta);
}

// 죽음 상태 업데이트 진행
void PlantMonster::updateDeleteDelay(float Delta) {
	deleteDelayTime += Delta;
	if (deleteDelayTime >= 2.3) 
		scene.DeleteObject(this);
}


////////////////////////////////////////////////////////////


// createPosition: 처음 생성될 때 스폰되는 위치
// terrainName: 현재 맵의 터레인 객체 이름
// appearFromGround: 활성화 시 땅 속에서 나옴
PlantMonster::PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, bool appearFromGround) {
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
	}
	else 
		position = tempPosition;
	
	// 고정형 몬스터이므로 생성 이후로는 시야 범위 업데이트를 진행하지 않는다.
	lookRange.Update(position, 10.0);

	// 디펜스 모드 일때는 중앙 건물 만을 공격하므로 생성 이후로는 회전각도 업데이트를 하지 않는다.
	if (defenseModeState) {
		if (auto centerBuilding = scene.Find("center_building"); centerBuilding) {
			XMFLOAT3 centerBuildingPosition = centerBuilding->GetPosition();
			float destRotation = Math::CalcDegree2D(position.z, position.x, centerBuildingPosition.z, centerBuildingPosition.x);
			rotation.y = destRotation;
		}
	}

	// 자기 소유의 hp 표시기 객체 추가
	hpIndicator = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER2);
}

// 모든 업데이트
void PlantMonster::Update(float Delta) {
	updateAnimation(Delta);
	updateHitBox();

	if (behaviorEnabledState) {
		if (currentState != PLANT_DEATH) {
			updateTargetDetect();
			updateIndicatorHP();
		}
		else
			updateDeleteDelay(Delta);
	}
	else
		updateLiftFromGround(Delta);
}

// 렌더링
void PlantMonster::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	RenderFBX(plantFBX, TEX.plantMonster);
	UpdatePickMatrix();

	hitBox.Render();
}

bool PlantMonster::CheckHit(XMFLOAT2& checkPosition, int damage) {
	if (currentState == PLANT_DEATH)
		return false;

	if (PickingUtil::PickByViewportOOBB(checkPosition, hitBox)) {
		currentHP -= damage;
		if (currentHP <= 0) {
			currentState = PLANT_DEATH;
			if (hpIndicator)
				scene.DeleteObject(hpIndicator);
		}
		return true;
	}

	return false;
}