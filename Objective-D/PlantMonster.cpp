#include "PlantMonster.h"
#include "HP_Indicator.h"

// 히트박스 업데이트 진행
void PlantMonster::updateHitBox() {
	hitBox.Update(position, size, rotation);
}

// 플레이어 접근 감지 진행
void PlantMonster::updatePlayerDetect() {
	
}

// hp 표시기 업데이트 진행
void PlantMonster::updateIndicatorHP() {
	if (hpIndicator) {
		hpIndicator->InputPosition(position, 3.0);
		hpIndicator->InputHP(currentHP, totalHP);
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

// 땅 속에서 나올때의 행동 실행 지연 업데이트 실행
void PlantMonster::updateBehaviorEnableDelay(float Delta) {
	behaviorEnableDelayTime += Delta;
	if (behaviorEnableDelayTime >= 1.0)
		behaviorEnabledState = true;
}

// 죽음 상태 업데이트 진행
void PlantMonster::updateDeleteDelay(float Delta) {
	deleteDelayTime += Delta;
	if (deleteDelayTime >= 3.0) 
		scene.DeleteObject(this);
}


////////////////////////////////////////////////////////////


// createPosition: 처음 생성될 때 스폰되는 위치
// terrainName: 현재 맵의 터레인 객체 이름
// appearFromGround: 활성화 시 땅 속에서 나옴
PlantMonster::PlantMonster(const XMFLOAT3& createPosition, const std::string& terrainName, bool appearFromGround) {
	// 원본에서 인스턴스 복사
	plantFBX.SelectFBXMesh(MESH.plantMonster);

	tempPosition = createPosition;
	behaviorEnabledState = !appearFromGround;

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
		terrainFloorHeight = tempPosition.y;

		//  땅에서 나오는 상태일 경우 높이를 땅 속으로 옮긴다.
		if (!behaviorEnabledState)
			position.y -= 10.0;
	}

	// 고정형 몬스터이므로 생성 이후로는 시야 범위 업데이트를 진행하지 않는다.
	lookRange.Update(position, 10.0);

	// 자기 소유의 hp 표시기 객체 추가
	hpIndicator = scene.AddObject(new HP_Indicator, "hpIndicator", LAYER2);
}

// 모든 업데이트
void PlantMonster::Update(float Delta) {
	updateAnimation(Delta);

	if (behaviorEnabledState) {
		if (currentState != PLANT_DEATH) {
			updateHitBox();
			updatePlayerDetect();
			updateIndicatorHP();
		}
		else
			updateDeleteDelay(Delta);
	}
	else
		updateBehaviorEnableDelay(Delta);
}

// 렌더링
void PlantMonster::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);
	RenderFBX(plantFBX, TEX.plantMonster);

	hitBox.Render();
}

// 죽을 시 자기 소유의 hp 표시기 객체를 즉시 삭제하고 죽음 상태를 활성화 한다.
void PlantMonster::GiveDamage(int hp) {
	if (currentState == PLANT_DEATH)
		return;

	currentHP -= hp;
	if (currentHP <= 0) {
		currentState = PLANT_DEATH;
		if (hpIndicator) 
			scene.DeleteObject(hpIndicator);
	}
}

// 죽은 상태일 경우 true를 리턴
bool PlantMonster::GetDeathState() {
	if (currentState == PLANT_DEATH)
		return true;
	return false;
}

// 히트 박스 얻기
OOBB PlantMonster::GetOOBB() {
	return hitBox;
}
