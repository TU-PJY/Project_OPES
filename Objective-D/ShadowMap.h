#pragma once
#include "DirectX_3D.h"

class ShadowMap {
private:
    // 리소스
    ComPtr<ID3D12Resource> mShadowMap;

    // 디스크립터 힙
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;
    ComPtr<ID3D12DescriptorHeap> mSrvHeap;      // SRV용
    ComPtr<ID3D12DescriptorHeap> mSamplerHeap;  // 샘플러용

    // CPU 핸들 (Create*View에 쓰는 핸들)
    D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle{};
    D3D12_CPU_DESCRIPTOR_HANDLE shadowSRV_CbvSrvUavHeapHandle{};
    D3D12_CPU_DESCRIPTOR_HANDLE shadowSamplerDescHeapHandle{};

    // (선택) GPU 핸들: 셰이더에서 바인딩할 때 필요
    D3D12_GPU_DESCRIPTOR_HANDLE shadowSRV_GpuHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE shadowSampler_GpuHandle{};

public:
    void Init(ID3D12Device*& device);

    // (선택) 외부 바인딩용 접근자
    D3D12_CPU_DESCRIPTOR_HANDLE DsvCpu() const { return shadowDSVHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE SrvGpu() const { return shadowSRV_GpuHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE SamplerGpu() const { return shadowSampler_GpuHandle; }
};

extern ShadowMap ShadowUtil;

