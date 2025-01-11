#include "ShaderUtil.h"

// 여기에 블랜드를 작성한다.

// 블랜드를 생성한다.
D3D12_BLEND_DESC Shader::CreateBlendState() {
	D3D12_BLEND_DESC BlendDesc;
	::ZeroMemory(&BlendDesc, sizeof(D3D12_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;

	// 0번 렌더 타겟의 블렌드 설정
	BlendDesc.RenderTarget[0].BlendEnable = TRUE; // 블렌드 활성화
	BlendDesc.RenderTarget[0].LogicOpEnable = FALSE;

	// 색상 블렌딩 설정 (SRC_ALPHA * Src + (1 - SRC_ALPHA) * Dest)
	BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // 소스 알파
	BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 반대 알파
	BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // 더하기 연산

	// 알파 블렌딩 설정
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// 쓰기 마스크 설정 (모든 채널 쓰기)
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;

	return(BlendDesc);
}
