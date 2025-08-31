#include "ConstantBuffers.h"
#include "CBVUtil.h"

#undef max
#undef min

CBV FlipCBV; // texture flip data
CBV BoolLightCBV;  // light bool data
CBV LightCBV;  // light data
CBV BoolFogCBV; // use fog data
CBV FogCBV; // fog data
CBV ShadowCBV; // shadow data

struct ShadowAABB {
	XMFLOAT3 minP;
	XMFLOAT3 maxP;
};

XMMATRIX Shadow_TexScaleBias() {
	// NDC[-1,1] → UV[0,1]
	return XMMATRIX(
		0.5f, 0, 0, 0,
		0, -0.5f, 0, 0,
		0, 0, 1, 0,
		0.5f, 0.5f, 0, 1
	);
};

// up이 라이트와 거의 평행일 때 보조 up을 선택해 안정화
XMVECTOR StableUpForDir(FXMVECTOR dir) {
	XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
	float dp = XMVectorGetX(XMVector3Dot(dir, worldUp));
	if (fabsf(dp) > 0.95f)
		return XMVectorSet(0, 0, 1, 0);
	return worldUp;
};

// 고정 방향광용: 한 번만 호출하면 끝
void BuildStaticShadowMatrices(const LIGHT_DATA& light, const ShadowAABB& bounds,
	float nearPlane, float farScale,
	SHADOW_MATRIX_DATA& outSm)
{
	// 1) 씬 커버리지
	XMVECTOR bmin = XMLoadFloat3(&bounds.minP);
	XMVECTOR bmax = XMLoadFloat3(&bounds.maxP);
	XMVECTOR center = XMVectorScale(XMVectorAdd(bmin, bmax), 0.5f);
	XMVECTOR extents = XMVectorScale(XMVectorSubtract(bmax, bmin), 0.5f);

	// 정사각 Ortho를 위해 가장 큰 반경으로 잡음 (해상도 낭비 최소화)
	float ex = XMVectorGetX(extents);
	float ey = XMVectorGetY(extents);
	float ez = XMVectorGetZ(extents);
	float radius = std::max(ex, std::max(ey, ez));

	// 2) 라이트 뷰 행렬 (라이트가 내려다보는 방향: -gLightDirection)
	XMVECTOR Ldir = XMVector3Normalize(XMLoadFloat3(&light.gLightDirection));
	XMVECTOR fwd = XMVectorNegate(Ldir);
	XMVECTOR up = StableUpForDir(fwd);
	// 라이트 카메라 위치는 중심 뒤쪽으로 약간 떨어뜨림
	XMVECTOR eye = XMVectorSubtract(center, XMVectorScale(fwd, radius * 2.0f));
	XMMATRIX  V = XMMatrixLookAtLH(eye, center, up);

	// 3) 라이트 직교 투영 (정사각형으로 설정)
	float width = radius * 2.0f;
	float height = radius * 2.0f;
	float zNear = std::max(0.01f, nearPlane);
	float zFar = radius * farScale;      // 예: farScale=4~8
	XMMATRIX  P = XMMatrixOrthographicLH(width, height, zNear, zFar);

	// 4) NDC→UV 변환
	XMMATRIX T = Shadow_TexScaleBias();

	// 5) CBV 데이터 채우기
	XMMATRIX VP = V * P;
	XMStoreFloat4x4(&outSm.LightViewProj, VP);
	XMStoreFloat4x4(&outSm.ShadowTex, T);
	outSm.ShadowBias = 0.0015f; // 시작값 (아크네 있으면 ↑, 분리되면 ↓)
}

XMFLOAT3 RGB_(int R, int G, int B);

// 상수버퍼로 사용할 버퍼 및 힙을 설정한다.
void CreateConstantBufferResource(ID3D12Device* Device) {
	// texture flipdata
	TEXTURE_FLIP_DATA TextureFlipData[4]{ {0, 0}, {1, 0}, {0, 1}, {1, 1} };
	ReserveConstantBuffer(FlipCBV, 4);
	for (int i = 0; i < 4; ++i)
		CBVUtil::Create(Device, &TextureFlipData[i], sizeof(TEXTURE_FLIP_DATA), FlipCBV, i);

	// light bool data
	USE_LIGHT_DATA UselightData[2]{ {0}, {1} };
	ReserveConstantBuffer(BoolLightCBV, 2);
	for (int i = 0; i < 2; ++i)
		CBVUtil::Create(Device, &UselightData[i], sizeof(USE_LIGHT_DATA), BoolLightCBV, i);

	// light data
	LIGHT_DATA LightData{
		{-10.0f, -20.0f, 10.0f },  // Light Position
		0.0, // padding1

		{1.0f, 0.9f, 0.8f },  // Light Color
		0.0, // padding2

		{0.25f, 0.25f, 0.25f },  // Ambient Light Color
		1.0,  // Shadow Strength
	};
	ReserveConstantBuffer(LightCBV, 1);
	CBVUtil::Create(Device, &LightData, sizeof(LIGHT_DATA), LightCBV);

	ShadowAABB terrainBounds{ { -1000, -100, -1000 }, { 1000, 100, 1000 } };
	SHADOW_MATRIX_DATA ShadowMatrix{};
	BuildStaticShadowMatrices(LightData, terrainBounds,
		/*nearPlane=*/0.1f,
		/*farScale=*/300.0f,
		ShadowMatrix);
	ReserveConstantBuffer(ShadowCBV, 1);
	CBVUtil::Create(Device, &ShadowMatrix, sizeof(SHADOW_MATRIX_DATA), ShadowCBV);

	// use fog data
	USE_FOG_DATA UseFogData[2]{ {0}, {1} };
	ReserveConstantBuffer(BoolFogCBV, 2);
	for (int i = 0; i < 2; ++i)
		CBVUtil::Create(Device, &UseFogData[i], sizeof(USE_FOG_DATA), BoolFogCBV, i);

	// fog data

	// map3
	// 0.68, 0.28, 0.1
	FOG_DATA FogData{
		{0.68, 0.28, 0.1}, // Fog Color
		0.0,   //   padding1

		500.0, // Fog Start
		{0.0, 0.0, 0.0}, // padding2

		900.0, // FogEnd
		{0.0, 0.0, 0.0} // padding3
	};
	ReserveConstantBuffer(FogCBV, 1);
	CBVUtil::Create(Device, &FogData, sizeof(FOG_DATA), FogCBV);
}

// RGB -> 1.0 색상값 변한 함수
XMFLOAT3 RGB_(int R, int G, int B) {
	XMFLOAT3 ReturnColor{};
	ReturnColor.x = 1.0 / 255.0 * float(R);
	ReturnColor.y = 1.0 / 255.0 * float(G);
	ReturnColor.z = 1.0 / 255.0 * float(B);

	return ReturnColor;
}