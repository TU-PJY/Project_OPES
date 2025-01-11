#include "RootConstantUtil.h"

// 루트상수를 쉐이더로 전달한다.
void RCUtil::Input(ID3D12GraphicsCommandList* CmdList, void* Data, int RootSignatureIndex, int BitValue, int Offset) {
	CmdList->SetGraphicsRoot32BitConstants(RootSignatureIndex, BitValue, Data, Offset);
}