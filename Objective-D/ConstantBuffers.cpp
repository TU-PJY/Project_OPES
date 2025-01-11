#include "ConstantBuffers.h"
#include "CBVUtil.h"

CBV FlipCBV; // texture flip data
CBV BoolLightCBV;  // light bool data
CBV LightCBV;  // light data
CBV BoolFogCBV; // use fog data
CBV FogCBV; // fog data

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
	LIGHT_DATA LighData{
		{5.0f, -5.0f, 5.0f },  // Light Position
		0.0, // padding1

		{1.0f, 0.9f, 0.8f },  // Light Color
		0.0, // padding2

		{0.2f, 0.2f, 0.2f },  // Ambient Light Color
		2.5,  // Shadow Strength
	};
	ReserveConstantBuffer(LightCBV, 1);
	CBVUtil::Create(Device, &LighData, sizeof(LIGHT_DATA), LightCBV);

	// use fog data
	USE_FOG_DATA UseFogData[2]{ {0}, {1} };
	ReserveConstantBuffer(BoolFogCBV, 2);
	for (int i = 0; i < 2; ++i)
		CBVUtil::Create(Device, &UseFogData[i], sizeof(USE_FOG_DATA), BoolFogCBV, i);

	// fog data
	FOG_DATA FogData{
		{0.5, 0.5, 0.5}, // Fog Color
		0.0,   //   padding1

		10.0, // Fog Start
		{0.0, 0.0, 0.0}, // padding2

		20.0, // FogEnd
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