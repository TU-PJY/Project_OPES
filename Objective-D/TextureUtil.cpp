#include "TextureUtil.h"
#include "RootConstants.h"
#include <iostream>

Texture::Texture(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, wchar_t* FileName, int Type, D3D12_FILTER FilterOption) {
	CreateTextureAndSRV(Device, CmdList, FileName, &Tex, &SRV, &Sampler, Type, FilterOption);
}

void Texture::ReleaseUploadBuffers() {
	if (UploadBuffer) 
		UploadBuffer->Release();
}

void Texture::Render(ID3D12GraphicsCommandList* CmdList) {
	ID3D12DescriptorHeap* DescriptorHeap[] = { SRV, Sampler };
	CmdList->SetDescriptorHeaps(_countof(DescriptorHeap), DescriptorHeap);
	CmdList->SetGraphicsRootDescriptorTable(SRV_INDEX, SRV->GetGPUDescriptorHandleForHeapStart());
	CmdList->SetGraphicsRootDescriptorTable(SAMPLER_INDEX, Sampler->GetGPUDescriptorHandleForHeapStart());
}

void Texture::CreateTextureAndSRV(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszTextureFilename, ID3D12Resource** ppd3dTexture, ID3D12DescriptorHeap** ppd3dSrvDescriptorHeap, ID3D12DescriptorHeap** ppd3dSamplerDescriptorHeap, int Type, D3D12_FILTER FilterOption) {
	// 텍스처 로드, 입력한 타입에 따라 다른 형식의 파일을 로드한다.
	if(Type == TEXTURE_TYPE_WIC)
		*ppd3dTexture = CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszTextureFilename, &UploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	else if(Type == TEXTURE_TYPE_DDS)
		*ppd3dTexture = CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszTextureFilename, &UploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// SRV 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(ppd3dSrvDescriptorHeap));

	// SRV 생성
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = (*ppd3dTexture)->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	pd3dDevice->CreateShaderResourceView(*ppd3dTexture, &srvDesc, (*ppd3dSrvDescriptorHeap)->GetCPUDescriptorHandleForHeapStart());

	// 샘플러 디스크립터 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
	samplerHeapDesc.NumDescriptors = 1;  // 샘플러는 하나만 생성
	samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // 셰이더에서 접근 가능하도록 설정
	pd3dDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(ppd3dSamplerDescriptorHeap));

	// 샘플러 생성
	D3D12_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = FilterOption;  // 비등방 필터링
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  // U 방향 랩 모드
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  // V 방향 랩 모드
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  // W 방향 랩 모드
	samplerDesc.MinLOD = 0;  // 최소 LOD
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;  // 최대 LOD
	pd3dDevice->CreateSampler(&samplerDesc, (*ppd3dSamplerDescriptorHeap)->GetCPUDescriptorHandleForHeapStart());
}

ID3D12Resource* Texture::CreateTextureResourceFromWICFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates) {
	ID3D12Resource* pd3dTexture = NULL;
	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA d3dSubresource;

	HRESULT hrc = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hrc)) {
		std::wcerr << L"Failed to initialize COM library." << std::endl;
		return nullptr;
	}

	HRESULT hr = DirectX::LoadWICTextureFromFileEx(pd3dDevice, pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, WIC_LOADER_DEFAULT, &pd3dTexture, decodedData, d3dSubresource);
	if (FAILED(hr)) {
		std::wcerr << L"Failed to load texture: " << pszFileName << L", HRESULT: " << std::hex << hr << std::endl;
	}

	if (pd3dTexture == nullptr) {
		std::wcerr << L"Texture resource is NULL after loading: " << pszFileName << std::endl;
		return nullptr;
	}

	D3D12_RESOURCE_DESC textureDesc = pd3dTexture->GetDesc();
	Width = static_cast<int>(textureDesc.Width);
	Height = static_cast<int>(textureDesc.Height);

	// 중간 버퍼 크기를 계산 (pd3dTexture가 NULL인지 다시 확인)
	UINT64 nBytes = GetRequiredIntermediateSize(pd3dTexture, 0, 1);
	if (nBytes == 0) {
		std::wcerr << L"Failed to calculate intermediate size for texture." << std::endl;
		return nullptr;
	}

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap에는 텍스쳐를 생성할 수 없음
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

	::UpdateSubresources(pd3dCommandList, pd3dTexture, *ppd3dUploadBuffer, 0, 0, 1, &d3dSubresource);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pd3dTexture;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	return(pd3dTexture);
}

ID3D12Resource* Texture::CreateTextureResourceFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates) {
	ID3D12Resource* pd3dTexture = NULL;
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;

	HRESULT hrc = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hrc)) {
		std::wcerr << L"Failed to initialize COM library." << std::endl;
		return nullptr;
	}

	HRESULT hr = DirectX::LoadDDSTextureFromFileEx(pd3dDevice, pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, &pd3dTexture, ddsData, vSubresources, &ddsAlphaMode, &bIsCubeMap);
	if (FAILED(hr)) {
		std::wcerr << L"Failed to load texture: " << pszFileName << L", HRESULT: " << std::hex << hr << std::endl;
	}

	if (pd3dTexture == nullptr) {
		std::wcerr << L"Texture resource is NULL after loading: " << pszFileName << std::endl;
		return nullptr;
	}

	UINT nSubResources = (UINT)vSubresources.size();
	UINT64 nBytes = GetRequiredIntermediateSize(pd3dTexture, 0, nSubResources);

	if (nBytes == 0) {
		std::wcerr << L"Failed to calculate intermediate size for texture." << std::endl;
		return nullptr;
	}

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap에는 텍스쳐를 생성할 수 없음
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

	::UpdateSubresources(pd3dCommandList, pd3dTexture, *ppd3dUploadBuffer, 0, 0, nSubResources, &vSubresources[0]);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pd3dTexture;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	return(pd3dTexture);
}