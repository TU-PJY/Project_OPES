#include "ShaderUtil.h"

// 여기에 파이프라인을 작성한다.

// 일반 출력 파이프라인을 생성한다.
void Shader::CreateDefaultPS(ID3D12Device* Device, ID3D12RootSignature* RootSignature) {
	ID3DBlob* VertexShaderBlob = NULL, * PixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = RootSignature;
	PipelineStateDesc.VS = CreateVertexShader(&VertexShaderBlob);
	PipelineStateDesc.PS = CreatePixelShader(&PixelShaderBlob);
	PipelineStateDesc.RasterizerState = CreateRasterizerState();
	PipelineStateDesc.BlendState = CreateBlendState();
	PipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	PipelineStateDesc.InputLayout = CreateInputLayout();
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	Device->CreateGraphicsPipelineState(&PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&PSDefault);

	if (VertexShaderBlob)
		VertexShaderBlob->Release();

	if (PixelShaderBlob)
		PixelShaderBlob->Release();

	if (PipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] PipelineStateDesc.InputLayout.pInputElementDescs;
}

// 깊이 검사가 비활성화된 파이프라인을 생성한다.
void Shader::CreateNoneDepthPS(ID3D12Device* Device, ID3D12RootSignature* RootSignature) {
	ID3DBlob* VertexShaderBlob = NULL, * PixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = RootSignature;
	PipelineStateDesc.VS = CreateVertexShader(&VertexShaderBlob);
	PipelineStateDesc.PS = CreatePixelShader(&PixelShaderBlob);
	PipelineStateDesc.RasterizerState = CreateRasterizerState();
	PipelineStateDesc.BlendState = CreateBlendState();

	// 깊이 검사를 비활성화한 스텐실을 생성한다.
	PipelineStateDesc.DepthStencilState = CreateImageDepthStencilState();

	PipelineStateDesc.InputLayout = CreateInputLayout();
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	Device->CreateGraphicsPipelineState(&PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&PSDepthNone);

	if (VertexShaderBlob)
		VertexShaderBlob->Release();

	if (PixelShaderBlob)
		PixelShaderBlob->Release();

	if (PipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] PipelineStateDesc.InputLayout.pInputElementDescs;
}

// 바운드박스 출력용 파이프라인을 생성한다.
void Shader::CreateWireframePS(ID3D12Device* Device, ID3D12RootSignature* RootSignature) {
	ID3DBlob* VertexShaderBlob = NULL, * PixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = RootSignature;
	PipelineStateDesc.VS = CreateVertexShader(&VertexShaderBlob);
	PipelineStateDesc.PS = CreatePixelShader(&PixelShaderBlob);
	PipelineStateDesc.RasterizerState = CreateBoundboxRasterizerState();
	PipelineStateDesc.BlendState = CreateBlendState();
	PipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	PipelineStateDesc.InputLayout = CreateInputLayout();
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	Device->CreateGraphicsPipelineState(&PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&PSWireframe);

	if (VertexShaderBlob)
		VertexShaderBlob->Release();

	if (PixelShaderBlob)
		PixelShaderBlob->Release();

	if (PipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] PipelineStateDesc.InputLayout.pInputElementDescs;
}

void Shader::CreateParticlePS(ID3D12Device* Device, ID3D12RootSignature* RootSignature) {
	ID3DBlob* VertexShaderBlob = NULL, * PixelShaderBlob = NULL, *GeometryShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = RootSignature;
	PipelineStateDesc.VS = CreateVertexShader(&VertexShaderBlob);
	PipelineStateDesc.PS = CreatePixelShader(&PixelShaderBlob);
	PipelineStateDesc.GS = CreateGeometryShader(&GeometryShaderBlob);
	PipelineStateDesc.RasterizerState = CreateRasterizerState();
	PipelineStateDesc.BlendState = CreateBlendState();
	PipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	PipelineStateDesc.InputLayout = CreateInputLayout();
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	Device->CreateGraphicsPipelineState(&PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&PSParticle);

	if (VertexShaderBlob)
		VertexShaderBlob->Release();

	if (PixelShaderBlob)
		PixelShaderBlob->Release();

	if (GeometryShaderBlob)
		GeometryShaderBlob->Release();

	if (PipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] PipelineStateDesc.InputLayout.pInputElementDescs;
}