#pragma once
#include <pch.h>

#include <dxgi1_4.h>
#include <d3d12.h>

namespace RHI
{
	struct SRhiState
	{
		IDXGIFactory2 *pFactory;
		IDXGIAdapter1 *pAdapter;
		ID3D12Device *pDevice;
		ID3D12CommandQueue *pCmdQueue;
		ID3D12Fence *pCmdQueueFence;
		HANDLE CmdQueueEvent;
		uint CmdQueueFenceValue;
		uint RenderTargetSize;
		uint GenericViewSize;
		ID3D12RootSignature *pRootSig;
		ID3D12PipelineState *pPipeline;
	};

	// ------------------------------------------------------------------------------------------ 
	// ------------------------------------------------------------------------------------------
	struct SWindowContext
	{
		DXGI_SWAP_CHAIN_DESC1 SwapchainDesc;
		IDXGISwapChain3 *pSwapchain;
		ID3D12DescriptorHeap *pRenderTargetHeap;
	};
}