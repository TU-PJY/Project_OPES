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
	bool CheckRayCollision(XMVECTOR& rayOrigin, XMVECTOR& rayDirection, const BoundSphere& Other);
	bool IsRightOfTarget(XMFLOAT3& ThisPosition, ObjectVector& Vector, XMFLOAT3& TargetPosition);
	void Vector_MoveForward(XMFLOAT3& Position, XMFLOAT3& Look, float Distance);
	void Vector_MoveStrafe(XMFLOAT3& Position, XMFLOAT3& Right, float Distance);
	void Vector_MoveUp(XMFLOAT3& Position, XMFLOAT3& Up, float Distance);
	void MoveForward(XMFLOAT3& Position, float RotationY, float MoveDistance);
	void MoveStrafe(XMFLOAT3& Position, float RotationY, float MoveDistance);
	void MoveUp(XMFLOAT3& Position, float MoveDistance);
	void GetOOBBAxis(FXMVECTOR& OrientationQuaternion, XMVECTOR& AxisX, XMVECTOR& AxisY, XMVECTOR& AxisZ);
	XMVECTOR ClosestPointOnOOBB(const OOBB& Box, FXMVECTOR& Point);
	void MoveWithSlide(XMFLOAT3& Position, float RotationY, float ForwardSpeed, float StrafeSpeed, BoundSphere& A, std::vector<OOBB>& B, float FrameTime);
	float CalcDistance2D(float FromX, float FromY, float ToX, float ToY);
	float CalcDistance3D(const XMFLOAT3& A, const XMFLOAT3& B);
	float CalcDegree2D(float FromX, float FromY, float ToX, float ToY);
	XMFLOAT3 CalcDegree3D(const XMFLOAT3& A, const XMFLOAT3& B);
	XMFLOAT3 CalcRadians3D(const XMFLOAT3& A, const XMFLOAT3& B);
	float CalcRadians2D(float FromX, float FromY, float ToX, float ToY);
	void Normalize2DAngleTo360(float& Degree);
	void LerpXMFLOAT3(XMFLOAT3& Value, XMFLOAT3& Dest, float Speed, float Delta);
	void MoveTowards(XMFLOAT3& CurrentPos, const XMFLOAT3& TargetPos, float Speed, float DeltaTime);
	void MoveTowardInfinity(XMFLOAT3& CurrentPos, const XMFLOAT3& TargetPos, float Speed, float DeltaTime);
	XMFLOAT3 CalcForwardOffset(const XMFLOAT3& Position, float DegreesY, float ForwardDistance, float HeightOffset);
}