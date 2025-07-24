#include "PoisonBall.h"
#include "CameraUtil.h"

void SendCenterBuildingPacket(int hp);

// 생성 시 날아갈 대상과의 각도를 계산한다.
PoisonBall::PoisonBall(const XMFLOAT3& createPosition, const XMFLOAT3& targetPosition, bool defenseMode) {
	position = createPosition;
	originPosition = createPosition;
	destPosition = targetPosition;
	Math::InitVector(vec);
	defenseModeState = defenseMode;

	XMFLOAT3 degrees = Math::CalcDegree3D(position, destPosition);
	moveAngleX = degrees.x;
	moveAngleY = degrees.y;
}

void PoisonBall::updateCollision() {
	if (disappearState)
		return;

	bool returnState{};
	size_t layerSize = scene.LayerSize(LAYER_PLAYER);
	for (int i = 0; i < layerSize; i++) {
		if (auto player = scene.ReferLayer(LAYER_PLAYER, i); player) {
			if (bs.CheckCollision(player->GetOOBB())) {
				// GiveDamage를 가진 1인칭 플레이어 객체만이 대미지를 받게 된다.
				player->GiveDamage(5);
				returnState = true;
			}
		}
	}

	if (returnState) {
		disappearState = true;
		return;
	}

	terrainUtil.InputPosition(position);
	if (terrainUtil.CheckCollision(GLOBAL.mapTerrain)) {
		disappearState = true;
		return;
	}
	
	for (auto& B : GLOBAL.mapOOBBdata) {
		if (B.CheckCollision(bs)) {
			disappearState = true;
			if (!GLOBAL.map1DefenseState)
				return;
		}
	}

	// 디펜스 모드에서는 센터 건물에 대미지 가함
	if (GLOBAL.map1DefenseState) {
		if (auto centerBuilding = scene.SearchLayer(LAYER1, "center_building"); centerBuilding) {
			if (bs.CheckCollision(centerBuilding->GetOOBB())) {
				//centerBuilding->GiveDamage(5);
				disappearState = true;
				SendCenterBuildingPacket(5);
			}
		}
	}
}

// 이동 업데이트
void PoisonBall::updateMove(float Delta) {
	if (disappearState)
		return;

	// 대상을 향해 날아간다
	Math::MoveInDirection(position, moveAngleY, moveAngleX, 30.0, Delta);
	bs.Update(position, 1.0);

	// 300.0이상 이동했다면 사라짐 상태 활성화
	float MoveDistance = Math::CalcDistance3D(position, originPosition);
	if (MoveDistance >= 300.0)
		disappearState = true;
}

// 사라지는 애니메여션 업데이트
void PoisonBall::updateDisappear(float Delta) {
	if (!disappearState)
		return;

	// 투명해짐과 동시에 커지면서 사라진다.
	opacity -= Delta * 4.0;
	size.x += Delta * 8.0;
	size.y += Delta * 8.0;
	size.z += Delta * 8.0;

	// 완전히 투명해지면 삭제
	if (opacity <= 0.0)
		scene.DeleteObject(this);
}

void PoisonBall::Update(float Delta) {
	updateMove(Delta);
	updateCollision();
	updateDisappear(Delta);
}

void PoisonBall::Render() {
	BeginRender();
	SetLightUse(DISABLE_LIGHT);
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(ScaleMatrix, size);
	Math::LookAt(RotateMatrix, vec, position, camera.GetPosition(), camera.GetUpVector());
	Render3D(SYSRES.BillboardMesh, TEX.poisonBall, opacity);

	bs.Render();
}
