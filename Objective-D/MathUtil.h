#pragma once
#include "DirectX_3D.h"
#include "CollisionUtil.h"

namespace Math {
	void UpdateVector(ObjectVector& VectorStruct, float Pitch, float Yaw, float Roll);
	void UpdateVector(ObjectVector& VectorStruct, XMFLOAT3& Rotation);
	void InitVector(ObjectVector& VectorStruct);
	void LookAt(XMFLOAT4X4& Matrix, ObjectVector& VectorStruct, XMFLOAT3& ThisPosition, XMFLOAT3& TargetPosition, XMFLOAT3& TargetUpVector);
	void BillboardLookAt(XMFLOAT4X4& Matrix, ObjectVector& VectorStruct, XMFLOAT3& ThisPosition, XMFLOAT3& TargetPosition);
	XMVECTOR CalcRayDirection(XMFLOAT3& Rotation);
	XMVECTOR CalcRayOrigin(XMFLOAT3& Position);
	bool CheckRayCollision(XMVECTOR& rayOrigin, XMVECTOR& rayDirection, const AABB& Other);
	bool CheckRayCollision(XMVECTOR& rayOrigin, XMVECTOR& rayDirection, const OOBB& Other);
	bool CheckRayCollision(XMVECTOR& rayOrigin, XMVECTOR& rayDirection, const Range& Other);
	bool IsRightOfTarget(XMFLOAT3& ThisPosition, ObjectVector& Vector, XMFLOAT3& TargetPosition);
	void Vector_MoveForward(XMFLOAT3& Position, XMFLOAT3& Look, float Distance);
	void Vector_MoveStrafe(XMFLOAT3& Position, XMFLOAT3& Right, float Distance);
	void Vector_MoveUp(XMFLOAT3& Position, XMFLOAT3& Up, float Distance);
	void MoveForward(XMFLOAT3& Position, float RotationY, float MoveDistance);
	void MoveStrafe(XMFLOAT3& Position, float RotationY, float MoveDistance);
	void MoveUp(XMFLOAT3& Position, float MoveDistance);
	float CalcDistance2D(float FromX, float FromY, float ToX, float ToY);
	float CalcDegree2D(float FromX, float FromY, float ToX, float ToY);
	float CalcRadians2D(float FromX, float FromY, float ToX, float ToY);
	void Normalize2DAngleTo360(float& Degree);
}