#if !defined(_ENGINE_RHI_DX12_H_)
#define _ENGINE_RHI_DX12_H_

#include <pch.h>

#include <Windows.h>
#include <dxgi1_4.h>
#include <d3d12.h>

namespace Engine
{
	struct CRhiDx12
	{
		CRhiDx12();
		~CRhiDx12();

		void WaitForQueue();

		IDXGIFactory2 *pDxgiFactory;
		IDXGIAdapter1 *pDxgiAdapter;
		ID3D12Device *pDevice;
		ID3D12CommandQueue *pCmdQueue;
		ID3D12Fence *pCmdFence;
		HANDLE CmdFenceEvent;
		uint CmdFenceValue;
		ID3D12CommandAllocator *pCmdAllocator;
		ID3D12GraphicsCommandList *pCmdList;
		uint RtvSize;
		uint GenericViewSize;
	};
	extern CRhiDx12 *pd3d;

	struct IDirect3DWindow
	{
		IDirect3DWindow() = default;
		IDirect3DWindow(uint uWidth, uint uHeight, const char *sTitle) { this->Initialize(uWidth, uHeight, sTitle); }
		bool Initialize(uint uWidth, uint uHeight, const char *uTitle);
		void Destroy();
		inline bool ShouldClose() { return m_hWnd == NULL; }
		inline bool InFocus() { return m_bInFocus; }

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

		/* ----- Members ----- */
	protected:
		HWND m_hWnd;
		uint m_uSize[2];
		bool m_bInFocus;
		IDXGISwapChain3 *m_pSwapChain;
		ID3D12Resource *m_pSwapChainImages[2];
		ID3D12DescriptorHeap *m_pRtvHeap;
	};
}

#endif // _ENGINE_RHI_DX12_H_