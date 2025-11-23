#include "Math.h"

// 현재 시점에서 앞으로 움직인다.
void MoveForward(XMFLOAT3& Position, float RotationY, float MoveDistance) {
	float Radians = XMConvertToRadians(RotationY);
	Position.x += sinf(Radians) * MoveDistance;
	Position.z += cosf(Radians) * MoveDistance;
}

// 현재 시점에서 옆으로 움직인다.
void MoveStrafe(XMFLOAT3& Position, float RotationY, float MoveDistance) {
	float Radians = XMConvertToRadians(RotationY);
	Position.x += cosf(Radians) * MoveDistance;
	Position.z -= sinf(Radians) * MoveDistance;
}

// 몬스터와 플레이어의 위치 계산
XMFLOAT3 CalcDegree3D(const XMFLOAT3& A, const XMFLOAT3& B) {
	XMFLOAT3 Direction = {
	   B.x - A.x,
	   B.y - A.y,
	   B.z - A.z
	};

	float Yaw = std::atan2(Direction.x, Direction.z);
	float Pitch = std::atan2(Direction.y, std::sqrt(Direction.x * Direction.x + Direction.z * Direction.z));
	float Roll = 0.0f;

	return XMFLOAT3(XMConvertToDegrees(Pitch), XMConvertToDegrees(Yaw), Roll);
}

// 각도를 360도 단위로 정규화
void Normalize2DAngleTo360(float& Degree) {
	Degree = fmod(Degree, 360.0f);
	if (Degree < 0.0f)
		Degree += 360.0f;
}
