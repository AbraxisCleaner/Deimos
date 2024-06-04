#include <pch.h>
#include "RhiDx12.h"

#ifdef _DEBUG
#	include <dxgidebug.h>
#	pragma comment (lib, "dxguid.lib")
#endif

SRhiState _rhi;
SRhiState *gRhi = &_rhi;
SRhiState *pd3d = gRhi; // Used because it's nicer to type 'pd3d' instead of 'gRhi' all the time.

void SetRhiState(SRhiState *pState) {
	gRhi = pState;
	pd3d = pState;
}

SRhiState *GetRhiState() {
	return gRhi;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool RHI::Initialize(bool bValidate)
{
#ifdef _DEBUG
	if (bValidate) {
		ID3D12Debug1 *pDebug1;
		::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug1));
		pDebug1->EnableDebugLayer();
		pDebug1->Release();
	}
#endif

	// @TODO: Choose an adapter.
	::CreateDXGIFactory1(IID_PPV_ARGS(&pd3d->pFactory));
	pd3d->pFactory->EnumAdapters1(0, &pd3d->pAdapter);

	::D3D12CreateDevice(pd3d->pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pd3d->pDevice));

	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	pd3d->pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&pd3d->pCmdQueue));

	pd3d->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pd3d->pCmdQueueFence));
	pd3d->CmdQueueFenceValue = 0;

	pd3d->CmdQueueEvent = ::CreateEventA(NULL, FALSE, FALSE, NULL);
	
	pd3d->RenderTargetSize = pd3d->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	pd3d->GenericViewSize = pd3d->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

void RHI::Release()
{
	WaitForRendering();

	::CloseHandle(pd3d->CmdQueueEvent);
	pd3d->pCmdQueueFence->Release();
	pd3d->pCmdQueue->Release();
	pd3d->pDevice->Release();
	pd3d->pAdapter->Release();
	pd3d->pFactory->Release();

#ifdef _DEBUG
	IDXGIDebug1 *pDebug1;
	::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug1));
	pDebug1->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	pDebug1->Release();
#endif
}

void RHI::WaitForRendering()
{
	if (pd3d->pCmdQueueFence->GetCompletedValue() != pd3d->CmdQueueFenceValue) {
		pd3d->pCmdQueueFence->SetEventOnCompletion(pd3d->CmdQueueFenceValue, pd3d->CmdQueueEvent);
		::WaitForSingleObject(pd3d->CmdQueueEvent, INFINITE);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool RHI::CreateWindowContext(HWND hwnd, SWindowContext *pOut)
{
	RECT Rect;
	::GetClientRect(hwnd, &Rect);

	ZeroThat(&pOut->SwapchainDesc);
	pOut->SwapchainDesc.BufferCount = 2;
	pOut->SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	pOut->SwapchainDesc.SampleDesc.Count = 1;
	pOut->SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	pOut->SwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pOut->SwapchainDesc.Width = (Rect.right - Rect.left);
	pOut->SwapchainDesc.Height = (Rect.bottom - Rect.top);
	
	IDXGISwapChain1 *pSwap1;
	auto hr = pd3d->pFactory->CreateSwapChainForHwnd(pd3d->pCmdQueue, hwnd, &pOut->SwapchainDesc, nullptr, nullptr, &pSwap1);
	if (FAILED(hr)) {
		return false;
	}

	pSwap1->QueryInterface(&pOut->pSwapchain);
	pSwap1->Release();

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.NumDescriptors = 2;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	
	pd3d->pDevice->CreateDescriptorHeap(&HeapDesc, &pOut->pRenderTargetHeap);
	auto Rtv = pOut->pRenderTargetHeap->GetCPUDescriptorHandleForHeapStart();

	ID3D12Resource *pResource;
	pOut->pSwapchain->GetBuffer(0, IID_PPV_ARGS(&pResource));
	pd3d->pDevice->CreateRenderTargetView(pResource, nullptr, Rtv);
	pResource->Release();

	Rtv.ptr += pd3d->RenderTargetSize;
	pOut->pSwapchain->GetBuffer(1, IID_PPV_ARGS(&pResource));
	pd3d->pDevice->CreateRenderTargetView(pResource, nullptr, Rtv);
	pResource->Release();

	return true;
}

void RHI::ReleaseWindowContext(SWindowContext *pCtx)
{
	RHI::WaitForRendering();
	pCtx->pRenderTargetHeap->Release();
	pCtx->pSwapchain->Release();
	ZeroThat(pCtx);
}

void RHI::ResizeWindowContext(SWindowContext *pCtx, uint NewWidth, uint NewHeight)
{
	if (!NewWidth || !NewHeight) {
		HWND hwnd;
		pCtx->pSwapchain->GetHwnd(&hwnd);

		RECT Rect;
		::GetClientRect(hwnd, &Rect);

		if (!NewWidth)
			pCtx->SwapchainDesc.Width = (Rect.right - Rect.left);
		else
			pCtx->SwapchainDesc.Width = NewWidth;
		if (!NewHeight)
			pCtx->SwapchainDesc.Height = (Rect.bottom - Rect.top);
		else
			pCtx->SwapchainDesc.Height = NewHeight;
	} else {
		pCtx->SwapchainDesc.Width = NewWidth;
		pCtx->SwapchainDesc.Height = NewHeight;
	}

	pCtx->pSwapchain->ResizeBuffers(2, NewWidth, NewHeight, pCtx->SwapchainDesc.Format, pCtx->SwapchainDesc.Flags);

	ID3D12Resource *pResource;
	pOut->pSwapchain->GetBuffer(0, IID_PPV_ARGS(&pResource));
	pd3d->pDevice->CreateRenderTargetView(pResource, nullptr, Rtv);
	pResource->Release();

	Rtv.ptr += pd3d->RenderTargetSize;
	pOut->pSwapchain->GetBuffer(1, IID_PPV_ARGS(&pResource));
	pd3d->pDevice->CreateRenderTargetView(pResource, nullptr, Rtv);
	pResource->Release();
}

