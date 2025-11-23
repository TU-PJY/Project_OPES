#include "ShadowMap.h"

ShadowMap ShadowUtil;

void ShadowMap::Init(ID3D12Device*& device) {
    const UINT SHADOW_SIZE = 2048;

    // 0) 디스크립터 힙 생성 + CPU/GPU 핸들 확보
    {
        // DSV 힙 (CPU 전용)
        D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeap{};
        dsvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvDescHeap.NumDescriptors = 1;
        dsvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        device->CreateDescriptorHeap(&dsvDescHeap, IID_PPV_ARGS(&mDsvHeap));
        shadowDSVHandle = mDsvHeap->GetCPUDescriptorHandleForHeapStart();

        // SRV 힙 (셰이더 가시) ? 이 힙을 커맨드리스트에 SetDescriptorHeaps로 바인딩해서 사용
        D3D12_DESCRIPTOR_HEAP_DESC srvDescHeap{};
        srvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvDescHeap.NumDescriptors = 1;
        srvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        device->CreateDescriptorHeap(&srvDescHeap, IID_PPV_ARGS(&mSrvHeap));
        shadowSRV_CbvSrvUavHeapHandle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
        shadowSRV_GpuHandle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();

        // Sampler 힙 (셰이더 가시)
        D3D12_DESCRIPTOR_HEAP_DESC sampDescHeap{};
        sampDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        sampDescHeap.NumDescriptors = 1;
        sampDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        device->CreateDescriptorHeap(&sampDescHeap, IID_PPV_ARGS(&mSamplerHeap));
        shadowSamplerDescHeapHandle = mSamplerHeap->GetCPUDescriptorHandleForHeapStart();
        shadowSampler_GpuHandle = mSamplerHeap->GetGPUDescriptorHandleForHeapStart();
    }

    // 1) 섀도우 맵 텍스처 (깊이 전용)
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = SHADOW_SIZE;
    texDesc.Height = SHADOW_SIZE;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32_TYPELESS; // typeless (DSV/SRV 겸용)
    texDesc.SampleDesc = { 1, 0 };
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearDepth = {};
    clearDepth.Format = DXGI_FORMAT_D32_FLOAT; // DSV용 포맷
    clearDepth.DepthStencil.Depth = 1.0f;
    clearDepth.DepthStencil.Stencil = 0;

    device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 초기 상태
        &clearDepth,
        IID_PPV_ARGS(&mShadowMap)         // ComPtr 멤버로 보관
    );

    // 2) DSV 생성 (깊이쓰기 뷰)
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        device->CreateDepthStencilView(mShadowMap.Get(), &dsvDesc, shadowDSVHandle);
    }

    // 3) SRV 생성 (섀도우 샘플용)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // SRV용 포맷
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        device->CreateShaderResourceView(mShadowMap.Get(), &srvDesc, shadowSRV_CbvSrvUavHeapHandle);
    }

    // 4) 비교 샘플러(PCF)
    {
        D3D12_SAMPLER_DESC cmp = {};
        cmp.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        cmp.AddressU = cmp.AddressV = cmp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        cmp.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        device->CreateSampler(&cmp, shadowSamplerDescHeapHandle);
    }
}
