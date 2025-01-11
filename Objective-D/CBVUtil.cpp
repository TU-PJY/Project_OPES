#include "CBVUtil.h"

// 쉐이더에 사용되는 상수 버퍼를 생성한다
// 쉐이더에 사용되는 상수 버퍼를 생성한다
void CBVUtil::Create(ID3D12Device* Device, void* Data, size_t DataSize, CBV& CBV_Struct, int CBV_Index) {
	UINT64 cbSize = (DataSize + 255) & ~255;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = cbSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&CBV_Struct.Buffer[CBV_Index])
	);

	void* pCbData = nullptr;  // void*로 매핑
	D3D12_RANGE readRange = { 0, 0 };  // 읽기 범위가 없으므로 0으로 설정
	CBV_Struct.Buffer[CBV_Index]->Map(0, &readRange, reinterpret_cast<void**>(&pCbData));

	memcpy(pCbData, Data, DataSize);

	CBV_Struct.Buffer[CBV_Index]->Unmap(0, nullptr);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&CBV_Struct.Heap[CBV_Index]));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = CBV_Struct.Buffer[CBV_Index]->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = cbSize;

	Device->CreateConstantBufferView(&cbvDesc, CBV_Struct.Heap[CBV_Index]->GetCPUDescriptorHandleForHeapStart());
}

// CBV를 쉐이더로 전송한다.
void CBVUtil::Input(ID3D12GraphicsCommandList* CmdList, CBV& CBV_Struct, int CBV_Index) {
	ID3D12DescriptorHeap* heaps[] = { CBV_Struct.Heap[CBV_Index] };
	CmdList->SetDescriptorHeaps(_countof(heaps), heaps);
	CmdList->SetGraphicsRootDescriptorTable(CBV_Struct.SignatureIndex, CBV_Struct.Heap[CBV_Index]->GetGPUDescriptorHandleForHeapStart());
}