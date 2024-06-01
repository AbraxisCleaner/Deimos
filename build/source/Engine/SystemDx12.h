#if !defined(_ENGINE_SYSTEM_DX12_H_)
#define _ENGINE_SYSTEM_DX12_H_

#include <pch.h>
#include <Engine/SystemWin64.h>

#include <d3d12.h>
#include <dxgi1_4.h>

namespace RHI
{
	struct SDx12State
	{
		ID3D12Device *pDevice;
		ID3D12CommandQueue *pCmdQueue;
		ID3D12Fence *pQueueFence;
		HANDLE FenceEvent;
		uint FenceValue;
		ID3D12CommandAllocator *pCmdAllocator;
		ID3D12GraphicsCommandList *pCmd;
		uint RenderTargetSize, GenericDescriptorSize;
	};
	extern SDx12State *gd3d;

	namespace Dx12
	{
		bool Initialize();
		void Shutdown();
		bool WaitForDrawing();
	}

	/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
	/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
	class CWindow
	{
	public:
		CWindow() = default;
		CWindow(int Width, int Height, const TCHAR *Title);
		
		void Release();
		void OnResize();

		void Show() const;
		void Hide() const;
		ID3D12Resource *GetCurrentRenderTargetResource() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTarget() const;

		inline void OnActivate(bool IsActive) { m_bInFocus = IsActive; } // NOTE(dalex): Should 'm_bInFocus' be called 'm_bIsActive'?

		inline bool ShouldClose() const { return m_bShouldClose; }
		inline bool InFocus() const { return m_bInFocus; }

		static void PumpEvents();

	private:
		HWND m_hWnd;
		uint m_uSize[2];
		bool m_bShouldClose;
		bool m_bInFocus;
		IDXGISwapChain3 *m_pSwapChain;
		ID3D12Resource *m_pSwapChainImages[2];
		ID3D12DescriptorHeap *m_pRenderTargetHeap;
	};
}

#endif // _ENGINE_SYSTEM_DX12_H_