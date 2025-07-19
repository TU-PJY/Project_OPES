#include "PoisonBall.h"
#include "CameraUtil.h"

// 생성 시 날아갈 대상과의 각도를 계산한다.
PoisonBall::PoisonBall(const XMFLOAT3& createPosition, const XMFLOAT3& targetPosition, const std::string& terrainName, bool defenseMode) {
	position = createPosition;
	originPosition = createPosition;
	destPosition = targetPosition;
	Math::InitVector(vec);
	defenseModeState = defenseMode;
	currentMapName = terrainName;

	XMFLOAT3 degrees = Math::CalcDegree3D(position, destPosition);
	moveAngleX = degrees.x;
	moveAngleY = degrees.y;

	if (auto terrain = scene.Find(terrainName); terrain) {
		mapBoundData = terrain->GetMapWallOOBB();
		currentTerrain = terrain;
	}
}

void PoisonBall::updateCollision() {
	if (disappearState)
		return;

	// 충돌시 사라지는 애니메이션과 함께 삭제된다.
	if (auto centerBuilding = scene.Find("center_building"); centerBuilding) {
		if (centerBuilding->GetOOBB().CheckCollision(bs)) {
			disappearState = true;
			return;
		}
	}

	size_t layerSize = scene.LayerSize(LAYER_PLAYER);
	for (int i = 0; i < layerSize; i++) {
		if (auto player = scene.FindMulti("player", LAYER_PLAYER, i); player) {
			if (bs.CheckCollision(player->GetOOBB())) {
				player->GiveDamage(5);
				disappearState = true;
				return;
			}
		}
	}

	if (currentTerrain) {
		terrainUtil.InputPosition(position);
		if (terrainUtil.CheckCollision(currentTerrain->GetTerrain())) {
			disappearState = true;
			return;
		}
	}

	for (auto& B : mapBoundData) {
		if (B.CheckCollision(bs)) {
			disappearState = true;
			return;
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
