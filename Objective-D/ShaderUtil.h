#pragma once
#include "DirectX_3D.h"

struct CB_GAMEOBJECT_INFO {
	XMFLOAT4X4						Matrix;
};

class Shader {
private:
	int								Ref = 0;

public:
	Shader() {}
	virtual ~Shader();

	void AddRef();
	void Release();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	D3D12_RASTERIZER_DESC CreateBoundboxRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	D3D12_DEPTH_STENCIL_DESC CreateImageDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** ShaderBlob);
	D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(WCHAR* FileName, LPCSTR Shadername, LPCSTR ShaderProfile, ID3DBlob** ShaderBlob);
	void CreateDefaultPS(ID3D12Device* Device, ID3D12RootSignature* RootSignature);
	void CreateWireframePS(ID3D12Device* Device, ID3D12RootSignature* RootSignature);
	void CreateParticlePS(ID3D12Device* Device, ID3D12RootSignature* RootSignature);
	void CreateNoneDepthPS(ID3D12Device* Device, ID3D12RootSignature* RootSignature);
	void OnPrepareRender(ID3D12GraphicsCommandList* CmdList, ID3D12PipelineState* PS);
	void RenderWireframe(ID3D12GraphicsCommandList* CmdList);
	void RenderDepthNone(ID3D12GraphicsCommandList* CmdList);
	void RenderDefault(ID3D12GraphicsCommandList* CmdList);
	void RenderParticle(ID3D12GraphicsCommandList* CmdList);
	virtual void CreateShaderVariables(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList) {}
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* CmdList, float elapsedTime) {}
	virtual void ReleaseShaderVariables() {}

protected:
	ID3D12PipelineState* PSDefault{};
	ID3D12PipelineState* PSDepthNone{};
	ID3D12PipelineState* PSWireframe{};
	ID3D12PipelineState* PSParticle{};
};