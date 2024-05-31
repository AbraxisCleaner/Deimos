#include <pch.h>
#include <Engine/Dx12RHI.h>
#include <Engine/Win64Layer.h>
#include <Engine/DebugLog.h>

#if defined(_DEBUG)
#	include <dxgidebug.h>
#endif

/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
Engine::Dx12::SDx12State _d3d;
Engine::Dx12::SDx12State *gd3d = &_d3d;

bool Engine::Dx12::Initialize()
{
#if defined(_DEBUG)
	ID3D12Debug *pDebug;
	::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));
	pDebug->EnableDebugLayer();
	pDebug->Release();
#endif

	::CreateDXGIFactory1(IID_PPV_ARGS(&gd3d->pFactory));
	gd3d->pFactory->EnumAdapters1(0, &gd3d->pAdapter);

	::D3D12CreateDevice(gd3d->pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&gd3d->pDevice));
	
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	gd3d->pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&gd3d->pCmdQueue));

	gd3d->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&gd3d->pFence));
	gd3d->FenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
	gd3d->FenceValue = 0;

	gd3d->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&gd3d->pCmdAllocator));
	gd3d->pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, gd3d->pCmdAllocator, nullptr, IID_PPV_ARGS(&gd3d->pCmd));

	gd3d->RtvSize = gd3d->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	gd3d->UavSize = gd3d->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

void Engine::Dx12::Shutdown()
{
	WaitForCmdQueue();

	gd3d->pCmd->Release();
	gd3d->pCmdAllocator->Release();

	::CloseHandle(gd3d->FenceEvent);
	gd3d->pFence->Release();
	gd3d->pCmdQueue->Release();
	gd3d->pDevice->Release();

	gd3d->pAdapter->Release();
	gd3d->pFactory->Release();

	ZeroThat(gd3d);
}

void Engine::Dx12::WaitForCmdQueue()
{
	if (gd3d->pFence->GetCompletedValue() != gd3d->FenceValue) {
		gd3d->pFence->SetEventOnCompletion(gd3d->FenceValue, gd3d->FenceEvent);
		::WaitForSingleObject(gd3d->FenceEvent, INFINITE);
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
bool bWndClassInitialized = false;

LRESULT CALLBACK Dx12WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	Engine::Dx12::CWindow *pWnd = (Engine::Dx12::CWindow *)::GetWindowLongPtrA(hwnd, GWLP_USERDATA);
	if (pWnd) {
		switch (umsg) {
		case WM_CLOSE: {
			pWnd->Release();
		} break;

		case WM_SIZE: {
			Engine::Dx12::WaitForCmdQueue();

			RECT Rect;
			::GetClientRect(hwnd, &Rect);
			pWnd->m_uSize[0] = (Rect.right - Rect.left);
			pWnd->m_uSize[1] = (Rect.bottom - Rect.top);

			pWnd->m_pSwapChainImages[1]->Release();
			pWnd->m_pSwapChainImages[0]->Release();
			pWnd->m_pSwapChain->ResizeBuffers(2, pWnd->m_uSize[0], pWnd->m_uSize[1], DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			pWnd->m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pWnd->m_pSwapChainImages[0]));
			pWnd->m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pWnd->m_pSwapChainImages[1]));

			auto Rtv = pWnd->m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
			gd3d->pDevice->CreateRenderTargetView(pWnd->m_pSwapChainImages[0], nullptr, Rtv);
			Rtv.ptr += gd3d->RtvSize;
			gd3d->pDevice->CreateRenderTargetView(pWnd->m_pSwapChainImages[0], nullptr, Rtv);
		} break;
		}
	}

	return ::DefWindowProcA(hwnd, umsg, wparam, lparam);
}

Engine::Dx12::CWindow::CWindow(uint Width, uint Height, const TCHAR *sTitle)
{
	if (!bWndClassInitialized) {
		WNDCLASSEXW Wc = {};
		Wc.cbSize = sizeof(Wc);
		Wc.cbWndExtra = sizeof(intptr_t);
		Wc.hInstance = Win64::gWin64->hInstance;
		Wc.hCursor = ::LoadCursor(Win64::gWin64->hInstance, IDC_ARROW);
		Wc.lpszClassName = L"Engine::Dx12::CWindow";
		Wc.lpfnWndProc = Dx12WndProc;
		Wc.style = CS_VREDRAW | CS_HREDRAW;
		::RegisterClassExW(&Wc);
		bWndClassInitialized = false;
	}

	MONITORINFO Monitor = { sizeof(Monitor) };
	::GetMonitorInfoA(::MonitorFromPoint({}, 1), &Monitor);

	RECT Rect = { 0, 0, (int)Width, (int)Height };
	::AdjustWindowRect(&Rect, WS_TILEDWINDOW, false);

	auto Px = ((Monitor.rcWork.right / 2) - (Rect.right / 2));
	auto Py = ((Monitor.rcWork.bottom / 2) - (Rect.bottom / 2));

	m_Hwnd = ::CreateWindowExW(0, L"Engine::Dx12::CWindow", sTitle, WS_TILEDWINDOW, Px, Py, Rect.right, Rect.bottom, nullptr, nullptr, Win64::gWin64->hInstance, nullptr);
	VERIFY(m_Hwnd);

	::SetWindowLongPtrA(m_Hwnd, GWLP_USERDATA, (intptr_t)this);

	::GetClientRect(m_Hwnd, &Rect);
	m_uSize[0] = (Rect.right - Rect.left);
	m_uSize[1] = (Rect.bottom - Rect.top);

	// -- SwapChain
	DXGI_SWAP_CHAIN_DESC1 Scd = {};
	Scd.BufferCount = 2;
	Scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Scd.SampleDesc.Count = 1;
	Scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Scd.Width = m_uSize[0];
	Scd.Height = m_uSize[1];
	
	IDXGISwapChain1 *pSwap1;
	gd3d->pFactory->CreateSwapChainForHwnd(gd3d->pCmdQueue, m_Hwnd, &Scd, nullptr, nullptr, &pSwap1);
	VERIFY(pSwap1);

	pSwap1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
	pSwap1->Release();

	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pSwapChainImages[0]));
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pSwapChainImages[1]));

	// -- Rtvs
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	HeapDesc.NumDescriptors = 2;
	gd3d->pDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_pRtvHeap));

	auto Rtv = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	gd3d->pDevice->CreateRenderTargetView(m_pSwapChainImages[0], nullptr, Rtv);
	Rtv.ptr += gd3d->RtvSize;
	gd3d->pDevice->CreateRenderTargetView(m_pSwapChainImages[1], nullptr, Rtv);
}

void Engine::Dx12::CWindow::Release()
{
	Dx12::WaitForCmdQueue();

	m_pRtvHeap->Release();
	m_pSwapChainImages[1]->Release();
	m_pSwapChainImages[0]->Release();
	m_pSwapChain->Release();

	::DestroyWindow(m_Hwnd);
	m_Hwnd = nullptr;
}

void Engine::Dx12::CWindow::SwapImages()
{
	Dx12::WaitForCmdQueue();
	m_pSwapChain->Present(0, 0);
}