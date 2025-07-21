#pragma once
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

// 현재 시점에서 앞으로 움직인다.
void MoveForward(XMFLOAT3& Position, float RotationY, float MoveDistance);

// 현재 시점에서 옆으로 움직인다.
void MoveStrafe(XMFLOAT3& Position, float RotationY, float MoveDistance);

// 몬스터와 플레이어의 위치 계산
XMFLOAT3 CalcDegree3D(const XMFLOAT3& A, const XMFLOAT3& B);

// 각도를 360도 단위로 정규화
void Normalize2DAngleTo360(float& Degree);