#pragma once
#include "ShaderUtil.h"

class Boundbox_Shader : public Shader {
public:
	Boundbox_Shader() {}
	virtual ~Boundbox_Shader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ShaderBlob);
};
