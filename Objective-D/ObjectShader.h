#pragma once
#include "ShaderUtil.h"

class Object_Shader : public Shader {
public:
	Object_Shader() {}
	virtual ~Object_Shader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ShaderBlob);
};
