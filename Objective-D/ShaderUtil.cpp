#include "ShaderUtil.h"

// 쉐이더
Shader::~Shader() {
	if (PSDefault)
		PSDefault->Release();
	PSDefault = NULL;

	ReleaseShaderVariables();
}

void Shader::AddRef() {
	Ref++;
}

void Shader::Release() {
	if (--Ref <= 0)
		delete this;
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout() {
	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	InputLayoutDesc.pInputElementDescs = NULL;
	InputLayoutDesc.NumElements = 0;

	return(InputLayoutDesc);
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob** ShaderBlob) {
	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.BytecodeLength = 0;
	ShaderByteCode.pShaderBytecode = NULL;

	return(ShaderByteCode);
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob** ShaderBlob) {
	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.BytecodeLength = 0;
	ShaderByteCode.pShaderBytecode = NULL;

	return(ShaderByteCode);
}

D3D12_SHADER_BYTECODE Shader::CreateGeometryShader(ID3DBlob** ShaderBlob) {
	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.BytecodeLength = 0;
	ShaderByteCode.pShaderBytecode = NULL;

	return(ShaderByteCode);
}

D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(WCHAR* FileName, LPCSTR Shadername, LPCSTR ShaderProfile, ID3DBlob** ShaderBlob) {
	UINT CompileFlags = 0;
#if defined(_DEBUG)
	CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DCompileFromFile(FileName, NULL, NULL, Shadername, ShaderProfile, CompileFlags, 0, ShaderBlob, NULL);

	D3D12_SHADER_BYTECODE ShaderByteCode;
	ShaderByteCode.BytecodeLength = (*ShaderBlob)->GetBufferSize();
	ShaderByteCode.pShaderBytecode = (*ShaderBlob)->GetBufferPointer();

	return(ShaderByteCode);
}

void Shader::OnPrepareRender(ID3D12GraphicsCommandList* CmdList, ID3D12PipelineState* PS) {
	if (PS)
		CmdList->SetPipelineState(PS);
}

// 와이어 프레임 파이프라인 적용, 깊이 검사 적용됨
void Shader::RenderWireframe(ID3D12GraphicsCommandList* CmdList) {
	OnPrepareRender(CmdList, PSWireframe);
}

// 깊이 검사를 해제한 파이프라인 적용
void Shader::RenderDepthNone(ID3D12GraphicsCommandList* CmdList) {
	OnPrepareRender(CmdList, PSDepthNone);
}

// 깊이 검사를 포함한 파이프라인 적용
void Shader::RenderDefault(ID3D12GraphicsCommandList* CmdList) {
	OnPrepareRender(CmdList, PSDefault);
}

void Shader::RenderParticle(ID3D12GraphicsCommandList* CmdList) {
	OnPrepareRender(CmdList, PSParticle);
}