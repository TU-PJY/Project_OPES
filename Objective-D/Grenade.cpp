#include "Grenade.h"
#include "MathUtil.h"
#include "Explosion.h"

Grenade::Grenade(const XMFLOAT3& createPosition, const XMFLOAT3& rotation) {
	position = createPosition;
	XMFLOAT3 direction = getDirectionFromRotation(rotation);
	velocity = XMFLOAT3(
		direction.x * 30.0,
		direction.y * 30.0,
		direction.z * 30.0
	);
	isStopped = false;
}

void Grenade::updateBound() {
	//grenadeBound.Update(position, 2.0);
	terrainUtil.InputPosition(position, 0.49);
}

void Grenade::updateMove(float Delta) {
	if (isStopped) return;

	// 중력 적용
	velocity.y += -gravity * Delta * 4.0;

	// 위치 이동
	position.x += velocity.x * Delta;
	position.y += velocity.y * Delta;
	position.z += velocity.z * Delta;

	// 충돌 검사
	if (terrainUtil.CheckCollision(GLOBAL.mapTerrain)) {
		XMFLOAT3 terrainNormal = terrainUtil.GetNormalAtPoint(GLOBAL.mapTerrain);
		XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&terrainNormal));

		// 보정: 법선 방향이 아래를 향하면 반전
		if (XMVectorGetY(normal) < 0.0f)
			normal = -normal;

		// 수류탄 속도 벡터
		XMVECTOR xmVelocity = XMLoadFloat3(&velocity);

		// 반사 처리
		XMVECTOR reflected = XMVector3Reflect(xmVelocity, normal);

		// 튕김 계수 적용 (y 방향만 반사)
		XMVECTOR normalComponent = XMVector3Dot(xmVelocity, normal) * normal;
		XMVECTOR tangentialComponent = xmVelocity - normalComponent;

		XMVECTOR bounce = -normalComponent * restitution;     // 튕김
		XMVECTOR slide = tangentialComponent * friction;     // 슬라이딩

		XMVECTOR newVelocity = slide + bounce;

		// 결과 저장
		XMStoreFloat3(&velocity, newVelocity);

		float bounceY = fabsf(XMVectorGetY(bounce));
		if (bounceY < 2.0f) {
			isStopped = true;
			velocity = XMFLOAT3(0, 0, 0);
			terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.5);
			return;
		}
		else
			terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.5);
	}
}

XMVECTOR Grenade::getNormalFromAngle(const XMFLOAT3& angleDeg) {
	// Pitch = X축 회전, Roll = Z축 회전 (Y축은 회전에 영향 없음)
	float pitchRad = XMConvertToRadians(angleDeg.x);
	float rollRad = XMConvertToRadians(angleDeg.z);

	// 기본 위쪽 벡터
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	// 회전 행렬 구성
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitchRad, 0.0f, rollRad);
	return XMVector3TransformNormal(up, rot);  // 회전된 법선
}

XMFLOAT3 Grenade::getDirectionFromRotation(const XMFLOAT3& angleDeg) {
	float pitch = XMConvertToRadians(angleDeg.x); // up/down
	float yaw   = XMConvertToRadians(angleDeg.y); // left/right

	// DirectX 기준: Forward = -Z, Up = +Y
	XMVECTOR dir = XMVectorSet(
		cosf(pitch) * sinf(yaw),     // x
		sinf(pitch),                // y (중요: pitch 증가 시 아래로 향해야 하므로 부호 반전)
		cosf(pitch) * cosf(yaw),     // z
		0.0f
	);

	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Normalize(dir));
	return result;
}

XMFLOAT3 Grenade::getEulerFromVelocity(const XMFLOAT3& velocity) {
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&velocity));

	float yaw = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));       // Y축 회전
	float pitch = asinf(-XMVectorGetY(dir));                        // X축 회전 (y가 위쪽이므로 음수)
	float roll = 0.0f;                                              // 롤은 탄체에는 필요 없음

	XMFLOAT3 angleDeg;
	angleDeg.x = XMConvertToDegrees(pitch);  // Pitch
	angleDeg.y = XMConvertToDegrees(yaw);    // Yaw
	angleDeg.z = XMConvertToDegrees(roll);   // Roll
	return angleDeg;
}

void Grenade::updateCollision() {
	if (isStopped)
		return;
}

void Grenade::Update(float Delta) {
	updateMove(Delta);
	updateBound();
	updateCollision();

	// 3초가 지나면 폭발한다.
	explodeTime += Delta;
	if (explodeTime >= 3.0) {
		scene.AddObject(new Explosion(position), "explosion", LAYER3);
		scene.DeleteObject(this);
	}
}

void Grenade::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, XMFLOAT3(3.0, 3.0, 3.0));
	Render3D(MESH.grenade, TEX.scifi);
}