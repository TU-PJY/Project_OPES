#pragma once
#include <DirectXMath.h>
using namespace DirectX;

// 여기에 CBV로 사용할 구조체를 정의한다.

// texture flip
struct TEXTURE_FLIP_DATA {
	int X_Flip;
	int Y_Flip;
};

// use light data
struct USE_LIGHT_DATA {
	int UseLight;
};

// light data
struct LIGHT_DATA {
	XMFLOAT3 gLightDirection;  // 12 bytes
	float padding1;                     // 4 bytes (패딩을 추가하여 16바이트 정렬)

	XMFLOAT3 gLightColor;      // 12 bytes
	float padding2;                     // 4 bytes (16바이트 정렬)

	XMFLOAT3 gAmbientColor;    // 12 bytes
	float gShadowStrength;              // 4 bytes (이로 인해 16바이트 정렬 완료)
};

// use fog data
struct USE_FOG_DATA {
	int UseFog;
};

// fog data
struct FOG_DATA {
	DirectX::XMFLOAT3 FogColor; // 12 bytes
	float Padding1;     // 4 bytes

	float FogStart;    // 4 bytes
	XMFLOAT3 Padding2;  // 12 bytes

	float FogEnd;      // 4 bytes
	XMFLOAT3 Padding3;  // 12 bytes
};

