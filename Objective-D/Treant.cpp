#include "Treant.h"
#include "CameraUtil.h"

Treant::Treant(const XMFLOAT3& createPosition, const std::string& terrainName) {
	position = createPosition;
	currentMapName = terrainName;

	// 빠른 터레인 - 오브젝트 통신을 위해 미리 객체 포인터를 구해놓고 사용한다.
	if (auto terrain = scene.Find(currentMapName); terrain)
		terrainPtr = terrain;
}

// 터레인 충돌 처리 업데이트
void Treant::updateTerrainCollision() {
	if (!inFrustum)
		return;

	if (terrainPtr) {
		terrainUtil.InputPosition(position);
		terrainUtil.ClampToTerrain(terrainPtr->GetTerrain(), position, 0.0);
	}
}

// 상태 별 인덱스 순서와 열거형 인덱스 순서가 같으므로 현재 상태를 직접 인덱스로 사용한다.
// 각 상태별로 별도의 모델이므로 상태에 해당되는 애니메이션을 리셋 한다.
void Treant::updateState() {
	if (prevState != currentState) {
		if (prevState != currentState) {
			treantFBX[currentState].ResetAnimation();
			prevState = currentState;
		}
	}
}

// 현재 상태에 해당하는 모델의 애니메이션을 업데이트 한다.
// 프러스텀 바깥에 있으면 재생 시간만 업데이트 한다.
void Treant::updateAnimation(float Delta) {
	treantFBX[currentState].UpdateAnimation(Delta, false);
}

// 프러스텀 바운드, 공격 바운드, 피격 바운드, 시야 바운드를 업데이트 한다.
void Treant::updateBound() {
	if (!inFrustum)
		return;

	frustumAABB.Update(XMFLOAT3(position.x, position.y + size.y, position.z), XMFLOAT3(6.0, 8.0, 6.0));
}

void Treant::Update(float Delta) {
	inFrustum = camera.CheckFrustum(frustumAABB);
	updateTerrainCollision();
	updateBound();
	updateState();
	updateAnimation(Delta);
}

void Treant::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, size);

	// 상태 전환 시 잔상이 남는것을 방지하기 위해 실제 상태별 렌더링은 늦게 평가된다.
	RenderFBX(treantFBX[renderState], TEX.treant);
	oobb.UpdateAnimated(treantFBX[renderState], TranslateMatrix, RotateMatrix, ScaleMatrix, 0);

	if (renderState != currentState)
		renderState = currentState;

	frustumAABB.Render();
	oobb.Render();
}
