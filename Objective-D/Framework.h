#pragma once
#include "DirectX_3D.h"
#include "TimerUtil.h"

class Framework {
public:
	Framework();

	bool Create(HINSTANCE hInstance, HWND hMainWnd);
	void Destroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateRenderTargetViewsAndDepthStencilView();

	void ChangeSwapChainState();

	void Init();
	void ReleaseObjects();

	void Update();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	LRESULT CALLBACK WindowsMessageFunc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SwitchToWindowMode(HWND hwnd);
	void SwitchToFullscreenMode(HWND hwnd);

	bool FullScreenState{};
	int							CLIENT_WIDTH;
	int							CLIENT_HEIGHT;

	XMFLOAT3 BackgroundColor{};

private:
	HINSTANCE					hInstance;
	HWND						hWnd;

	IDXGIFactory4* DxgiFactory;
	IDXGISwapChain3* DxgiSwapChain;
	ID3D12Device* Device;

	bool						MSAA_4x_Option = false;
	UINT						MSAA_4x_QualityLevels = 8;

	static const UINT			SwapChainBuffers = 2;
	UINT						SwapChainBufferIndex;

	ID3D12Resource* SwapChainBackBuffers[SwapChainBuffers];
	ID3D12DescriptorHeap* RtvDescriptorHeap;
	UINT						RtvDescriptorIncrementSize;

	ID3D12Resource* DepthStencilBuffer;
	ID3D12DescriptorHeap* DsvDescriptorHeap;
	UINT						DsvDescriptorIncrementSize;

	ID3D12CommandAllocator* CmdAllocator;
	ID3D12CommandQueue* CmdQueue;
	ID3D12GraphicsCommandList* CmdList;

	ID3D12Fence* m_pd3dFence;
	UINT64						FenceValues[SwapChainBuffers];
	HANDLE						FenceEvent;

#if defined(_DEBUG)
	ID3D12Debug* DebugController;
#endif

	Timer						Timer;
	_TCHAR						FrameRate[50];

public:
	TCHAR						WindowName[50]{};
	int							TitleNameLength{};
};
