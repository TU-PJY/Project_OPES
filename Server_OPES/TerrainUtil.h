#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

float GetHeightAtPosition(std::vector<XMFLOAT3>& data, float x, float z);
bool IsPointInTriangle(XMFLOAT2& pt, XMFLOAT2& v0, XMFLOAT2& v1, XMFLOAT2& v2);
float ComputeHeightOnTriangle(XMFLOAT3& pt, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2);
