#if !defined(_ENGINE_DX12_RHI_H_)
#define _ENGINE_DX12_RHI_H_

#include <pch.h>

#include <dxgi1_4.h>
#include <d3d12.h>

namespace Engine::Dx12
{
	struct SDx12State
	{
		IDXGIFactory2 *pFactory;
		IDXGIAdapter1 *pAdapter;
		ID3D12Device *pDevice;
		ID3D12CommandQueue *pCmdQueue;
		ID3D12Fence *pFence;
		HANDLE FenceEvent;
		uint FenceValue;
		ID3D12CommandAllocator *pCmdAllocator;
		ID3D12GraphicsCommandList *pCmd;
		uint RtvSize;
		uint UavSize;
	};
	extern SDx12State *gd3d;

	bool Initialize();
	void Shutdown();

	void WaitForCmdQueue();

	/* -------------------------------------------------- */
	struct CWindow 
	{
		CWindow() = default;
		CWindow(uint Width, uint Height, const TCHAR *sTitle);

		void Release();
		void SwapImages();
		
		inline bool ShouldClose() const { return (m_Hwnd) ? true : false; }
		inline ID3D12Resource *GetActiveBackbufferImage() const { return m_pSwapChainImages[m_pSwapChain->GetCurrentBackBufferIndex()]; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetActiveBackbufferTarget() const { auto Rtv = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart(); Rtv.ptr += (gd3d->RtvSize * m_pSwapChain->GetCurrentBackBufferIndex()); return Rtv; }

		HWND m_Hwnd;
		uint m_uSize[2];
		IDXGISwapChain3 *m_pSwapChain;
		ID3D12Resource *m_pSwapChainImages[2];
		ID3D12DescriptorHeap *m_pRtvHeap;
	};
}

#endif // _ENGINE_DX12_RHI_H_