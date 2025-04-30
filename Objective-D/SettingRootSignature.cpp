#include "SettingRootSignature.h"
#include "Scene.h"
#include "RootConstants.h"
#include "ConstantBuffers.h"

void CompleteRootSignatureSetting(ID3D12Device*& Device, ID3D12RootSignature*& RootSignature, std::vector<D3D12_ROOT_PARAMETER>& RootParam);

// 오브젝트 쉐이더 루트 시그니처 설정
ID3D12RootSignature* Scene::CreateObjectShaderSignature(ID3D12Device* Device) {
	ID3D12RootSignature* GraphicsRootSignature{};
	D3D12_DESCRIPTOR_RANGE Range{};
	std::vector<D3D12_ROOT_PARAMETER> RootParameters(9);

	SetRoot(RootParameters, 20, 0, 0, GAME_OBJECT_INDEX);   // b0, cbGameObjectInfo

	SetRoot(RootParameters, 35, 1, 1, CAMERA_INDEX);        // b1, cbCameraInfo

	SetCBV(Range, RootParameters, 2, 2, FlipCBV);           // b2, cbFlipInfo

	SetCBV(Range, RootParameters, 3, 3, BoolLightCBV);      // b3, cbLightUseInfo

	SetCBV(Range, RootParameters, 4, 4, LightCBV);          // b4, cbLightInfo

	SetCBV(Range, RootParameters, 5, 5, BoolFogCBV);            // b5, cbFogUseInfo

	SetCBV(Range, RootParameters, 6, 6, FogCBV);            // b6, cbFogInfo

	SetSRV(Range, RootParameters, 0, 7, SRV_INDEX_3D);         // t0, SRV

	SetSampler(Range, RootParameters, 0, 8, SAMPLER_INDEX_3D); // s0, Sampler

	CompleteRootSignatureSetting(Device, GraphicsRootSignature, RootParameters);

	return GraphicsRootSignature;
}

// 이미지 쉐이더 루트 시그니처 설정
ID3D12RootSignature* Scene::CreateImageShaderSignature(ID3D12Device* Device) {
	ID3D12RootSignature* GraphicsRootSignature{};
	D3D12_DESCRIPTOR_RANGE Range{};
	std::vector<D3D12_ROOT_PARAMETER> RootParameters(5);

	SetRoot(RootParameters, 20, 0, 0, GAME_OBJECT_INDEX);   // b0, cbGameObjectInfo

	SetRoot(RootParameters, 35, 1, 1, CAMERA_INDEX);        // b1, cbCameraInfo

	SetCBV(Range, RootParameters, 2, 2, FlipCBV);           // b2, cbFlipInfo

	SetSRV(Range, RootParameters, 0, 3, SRV_INDEX_2D);         // t0, SRV

	SetSampler(Range, RootParameters, 0, 4, SAMPLER_INDEX_2D); // s0, Sampler

	CompleteRootSignatureSetting(Device, GraphicsRootSignature, RootParameters);

	return GraphicsRootSignature;
}


//////////////////////////////////////////////////////////


void CompleteRootSignatureSetting(ID3D12Device*& Device, ID3D12RootSignature*& RootSignature, std::vector<D3D12_ROOT_PARAMETER>& RootParam) {
	// 루트 시그니처 설정
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	RootSignatureDesc.NumParameters = RootParam.size(); //_countof(RootParameters);
	RootSignatureDesc.pParameters = RootParam.data();
	RootSignatureDesc.NumStaticSamplers = 0;
	RootSignatureDesc.pStaticSamplers = NULL;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* SignatureBlob = NULL;
	ID3DBlob* ErrorBlob = NULL;
	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &SignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, SignatureBlob->GetBufferPointer(), SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&RootSignature);

	if (SignatureBlob) SignatureBlob->Release();
	if (ErrorBlob) ErrorBlob->Release();
}