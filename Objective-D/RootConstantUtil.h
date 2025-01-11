#pragma once
#include "DirectX_3D.h"

namespace RCUtil {
	void Input(ID3D12GraphicsCommandList* CmdList, void* Data, int RootSignatureIndex, int BitValue, int Offset);
}