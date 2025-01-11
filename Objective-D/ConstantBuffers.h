#pragma once
#include <d3d12.h>
#include <vector>
#include "ConstantBufferData.h"

typedef struct {
	std::vector<ID3D12DescriptorHeap*> Heap;
	std::vector<ID3D12Resource*> Buffer;
	int SignatureIndex;
}CBV;

// flip CBV
extern CBV FlipCBV;

// light bool info CBV
extern CBV BoolLightCBV;

// light CBV
extern CBV LightCBV;

// fog bool info cbv
extern CBV BoolFogCBV;

// fog cbv
extern CBV FogCBV;


// CBV 생성 관련 함수들
void CreateConstantBufferResource(ID3D12Device* Device);
inline void ReserveConstantBuffer(CBV& CBV_Struct, int ReserveSize) {
	CBV_Struct.Buffer.reserve(ReserveSize);
	CBV_Struct.Heap.reserve(ReserveSize);
}