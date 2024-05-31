#if !defined(_ENGINE_SYSTEM_DX12_H_)
#define _ENGINE_SYSTEM_DX12_H_

#include <pch.h>
#include <Engine/SystemWin64.h>

#include <d3d12.h>
#include <dxgi1_4.h>

namespace System
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

	class CWindow
	{
	public:
		CWindow() = default;
		CWindow(int Width, int Height, const TCHAR *Title);
		
		void Release();
		void Show() const;
		void Hide() const;
		ID3D12Resource *GetCurrentResource() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTarget() const;
		inline bool ShouldClose() const { return m_ShouldClose; }
		inline bool InFocus() const { return m_InFocus; }

		static void PumpEvents();

	public:
		HWND m_Hwnd;
		uint m_Size[2];
		bool m_ShouldClose;
		bool m_InFocus;
		IDXGISwapChain3 *m_pSwapChain;
		ID3D12Resource *m_pSwapChainImages[2];
		ID3D12DescriptorHeap *m_pRenderTargetHeap;
	};
}

#endif // _ENGINE_SYSTEM_DX12_H_