#pragma once
#include "DirectX_3D.h"
#include "ConstantBuffers.h"

namespace CBVUtil {
	void Create(ID3D12Device* Device, void* Data, size_t DataSize, CBV& CBV_Struct, int CBV_Index=0);
	void Input(ID3D12GraphicsCommandList* CmdList, CBV& CBV_Struct, int CBV_Index=0);
}