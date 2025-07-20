#include "TerrainUtil.h"
#include <atomic>
#include <optional>
#include <thread>
#include <algorithm>
#include <execution>

float GetHeightAtPosition(std::vector<XMFLOAT3>& data, float x, float z) {
	size_t size = data.size();

	std::atomic<bool> found = false;
	std::optional<float> result;

	std::vector<size_t> indices;
	for (size_t i = 0; i < size; i += 3)
		indices.push_back(i); // 병렬 loop를 위한 index 리스트

	std::for_each(std::execution::par, indices.begin(), indices.end(), [&](size_t i) {
		if (found.load(std::memory_order_relaxed)) return;

		XMFLOAT3 v0 = data[i];
		XMFLOAT3 v1 = data[i + 1];
		XMFLOAT3 v2 = data[i + 2];

		if (IsPointInTriangle(XMFLOAT2(x, z), XMFLOAT2(v0.x, v0.z), XMFLOAT2(v1.x, v1.z), XMFLOAT2(v2.x, v2.z))) {
			float h = ComputeHeightOnTriangle(XMFLOAT3(x, 0, z), v0, v1, v2);
			if (!found.exchange(true)) {
				result = h;
			}
		}
	});

	return result.has_value() ? result.value() : 0.0f;
}

bool IsPointInTriangle(XMFLOAT2& pt, XMFLOAT2& v0, XMFLOAT2& v1, XMFLOAT2& v2) {
	float d00 = (v1.x - v0.x) * (v1.x - v0.x) + (v1.y - v0.y) * (v1.y - v0.y);
	float d01 = (v1.x - v0.x) * (v2.x - v0.x) + (v1.y - v0.y) * (v2.y - v0.y);
	float d11 = (v2.x - v0.x) * (v2.x - v0.x) + (v2.y - v0.y) * (v2.y - v0.y);
	float d20 = (pt.x - v0.x) * (v1.x - v0.x) + (pt.y - v0.y) * (v1.y - v0.y);
	float d21 = (pt.x - v0.x) * (v2.x - v0.x) + (pt.y - v0.y) * (v2.y - v0.y);

	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;
	return (u >= 0) && (v >= 0) && (w >= 0);
}

float ComputeHeightOnTriangle(XMFLOAT3& pt, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2) {
	XMVECTOR p = XMLoadFloat3(&pt);
	XMVECTOR a = XMLoadFloat3(&v0);
	XMVECTOR b = XMLoadFloat3(&v1);
	XMVECTOR c = XMLoadFloat3(&v2);

	XMVECTOR n = XMVector3Cross(b - a, c - a);
	n = XMVector3Normalize(n);

	float d = -XMVectorGetX(XMVector3Dot(n, a));
	float height = -(XMVectorGetX(XMVector3Dot(n, XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f) * p)) + d) / XMVectorGetY(n);

	return height;
}