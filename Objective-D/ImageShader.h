#pragma once
#include "ShaderUtil.h"

class Image_Shader : public Shader {
public:
	Image_Shader() {}
	virtual ~Image_Shader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ShaderBlob);
};
