#include <pch.h>
#include <Engine/Debug.h>
#include <Engine/RhiDx12.h>

#if defined(_DEBUG)
#	include <dxgidebug.h>
#endif

/* --------------------------------------------------------------------------------------------------------- */
Engine::CRhiDx12::CRhiDx12()
{
#if defined(_DEBUG)
	ID3D12Debug1 *pDebug1;
	::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug1));
	pDebug1->EnableDebugLayer();
	pDebug1->Release();
#endif

	::CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory));
	pDxgiFactory->EnumAdapters1(0, &pDxgiAdapter);

	::D3D12CreateDevice(pDxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
	
	D3D12_COMMAND_QUEUE_DESC CmdQueueDesc = {};
	CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	pDevice->CreateCommandQueue(&CmdQueueDesc, IID_PPV_ARGS(&pCmdQueue));

	pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pCmdFence));
	CmdFenceEvent = ::CreateEventW(NULL, false, false, NULL);
	CmdFenceValue = 0;

	pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCmdAllocator));
	pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCmdAllocator, NULL, IID_PPV_ARGS(&pCmdList));

	RtvSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	GenericViewSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// -- WndClass
	::WNDCLASSEXW Wc = {};
	Wc.cbSize = sizeof(Wc);
	Wc.cbWndExtra = sizeof(intptr_t);
	Wc.hInstance = ::GetModuleHandleW(NULL);
	Wc.hCursor = ::LoadCursorW(Wc.hInstance, IDC_ARROW);
	Wc.lpszClassName = L"IDirect3DWindow";
	Wc.lpfnWndProc = CDirect3DWindow::WndProc;
	Wc.style = CS_VREDRAW | CS_HREDRAW;
	ASSERT(RegisterClassExW(&Wc));
}

Engine::CRhiDx12::~CRhiDx12()
{
	WaitForQueue();

	pCmdAllocator->Release();
	pCmdList->Release();

	pCmdFence->Release();
	pCmdQueue->Release();
	pDevice->Release();

	pDxgiAdapter->Release();
	pDxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1 *pDebug1;
	::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug1));
	pDebug1->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	pDebug1->Release();
#endif
}

void Engine::CRhiDx12::WaitForQueue()
{
	if (pCmdFence->GetCompletedValue() != CmdFenceValue) {
		pCmdFence->SetEventOnCompletion(CmdFenceValue, CmdFenceEvent);
		::WaitForSingleObject(CmdFenceEvent, INFINITE);
	}
}

/* --------------------------------------------------------------------------------------------------------- */
bool Engine::CDirect3DWindow::Initialize(uint uWidth, uint uHeight, const char *sTitle)
{
	::MONITORINFO Monitor = { sizeof(Monitor) };
	::GetMonitorInfoA(::MonitorFromPoint({}, 1), &Monitor);
	
	::RECT Rect = { 0, 0, uWidth, uHeight };
	::AdjustWindowRect(&Rect, WS_TILEDWINDOW, false);

	auto Px = ((Monitor.rcWork.right / 2) - (Rect.right / 2));
	auto Py = ((Monitor.rcWork.bottom / 2) - (Rect.bottom / 2));

	m_hWnd = ::CreateWindowExA(0, "IDirect3DWindow", sTitle, WS_TILEDWINDOW, Px, Py, Rect.right, Rect.bottom, nullptr, nullptr, ::GetModuleHandleW(nullptr), nullptr);
	::SetWindowLongPtrA(m_hWnd, GWLP_USERDATA, (intptr_t)this);

	::GetClientRect(m_hWnd, &Rect);
	m_uSize[0] = (Rect.right - Rect.left);
	m_uSize[1] = (Rect.bottom - Rect.top);

	m_bInFocus = true;

	// -- SwapChain
	DXGI_SWAP_CHAIN_DESC1 Scd = {};
	Scd.BufferCount = 2;
	Scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Scd.SampleDesc.Count = 1;
	Scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Scd.Width = m_uSize[0];
	Scd.Height = m_uSize[1];
	
	IDXGISwapChain1 *pSwapChain1;
	pd3d->pDxgiFactory->CreateSwapChainForHwnd(pd3d->pCmdQueue, m_hWnd, &Scd, nullptr, nullptr, &pSwapChain1);
	pSwapChain1->QueryInterface(&m_pSwapChain);
	pSwapChain1->Release();

	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pSwapChainImages[0]));
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pSwapChainImages[1]));

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.NumDescriptors = 2;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	pd3d->pDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_pRtvHeap));

	auto Rtv = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	pd3d->pDevice->CreateRenderTargetView(m_pSwapChainImages[0], nullptr, Rtv);
	Rtv.ptr += pd3d->RtvSize;
	pd3d->pDevice->CreateRenderTargetView(m_pSwapChainImages[1], nullptr, Rtv);

	return true;
}

void Engine::CDirect3DWindow::Destroy()
{
	pd3d->WaitForQueue();
	m_pRtvHeap->Release();
	m_pSwapChainImages[1]->Release();
	m_pSwapChainImages[0]->Release();
	m_pSwapChain->Release();
	::DestroyWindow(m_hWnd);
	m_hWnd = NULL;
}

LRESULT CALLBACK Engine::CDirect3DWindow::WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	CDirect3DWindow*pWnd = (CDirect3DWindow*)::GetWindowLongPtrA(hwnd, GWLP_USERDATA);
	if (pWnd) {
		switch (umsg) {
		case WM_CLOSE: {
			pWnd->Destroy();
		} break;

		case WM_SIZE: {
			::RECT Rect;
			::GetClientRect(hwnd, &Rect);
			pWnd->m_uSize[0] = (Rect.right - Rect.left);
			pWnd->m_uSize[1] = (Rect.bottom - Rect.top);

			pWnd->m_pSwapChainImages[1]->Release();
			pWnd->m_pSwapChainImages[0]->Release();
			pWnd->m_pSwapChain->ResizeBuffers(2, pWnd->m_uSize[0], pWnd->m_uSize[1], DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			pWnd->m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pWnd->m_pSwapChainImages[0]));
			pWnd->m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pWnd->m_pSwapChainImages[1]));
			auto Rtv = pWnd->m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
			pd3d->pDevice->CreateRenderTargetView(pWnd->m_pSwapChainImages[0], nullptr, Rtv);
			Rtv.ptr += pd3d->RtvSize;
			pd3d->pDevice->CreateRenderTargetView(pWnd->m_pSwapChainImages[1], nullptr, Rtv);
		} break;

		case WM_ACTIVATEAPP: {
			if (wparam)
				pWnd->m_bInFocus = true;
			else
				pWnd->m_bInFocus = false;
		} break;
		}
	}
	return ::DefWindowProcA(hwnd, umsg, wparam, lparam);
}