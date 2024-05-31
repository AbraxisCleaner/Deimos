#if !defined(_ENGINE_RHI_DX12_H_)
#define _ENGINE_RHI_DX12_H_

#include <pch.h>

#include <dxgi1_4.h>
#include <d3d12.h>

namespace Engine
{
	/* ------------------------------------------------------- */
    struct IRhiD3D12
	{
        IRhiD3D12();
        ~IRhiD3D12();
		void WaitForGpu();

    public:
        IDXGIFactory2 *pDxgiFactory;
        IDXGIAdapter1 *pDxgiAdapter;
        ID3D12Device *pDevice;
        ID3D12CommandQueue *pCmdQueue;
        ID3D12Fence *pFence;
        HANDLE FenceEvent;
        uint FenceValue;
        uint RtvDescriptorSize;
        uint GenericDescriptorSize;
        ID3D12CommandAllocator *pCmdAllocator;
        ID3D12GraphicsCommandList *pCmd;
    };
    extern IRhiD3D12 *gd3d;

    /* ------------------------------------------------------- */
    struct IRhiWindow
    {
        IRhiWindow() = default;
        IRhiWindow(uint Width, uint Height, str_t Title);
        void Release();

        inline D3D12_CPU_DESCRIPTOR_HANDLE GetRtvDescriptor(uint FrameIndex) { auto Rtv = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart(); Rtv.ptr += FrameIndex * gd3d->RtvDescriptorSize; }
        inline D3D12_CPU_DESCRIPTOR_HANDLE GetRtvForActiveFrame() { return GetRtvDescriptor(m_pSwapChain->GetCurrentBackBufferIndex()); }

    public:
        uint m_uSize[2];

    private:
        HWND m_Hwnd;
        IDXGISwapChain3 *m_pSwapChain;
        ID3D12Resource *m_pSwapChainImages[2];
        ID3D12DescriptorHeap *m_pRtvHeap;
    };
}

#endif // _ENGINE_RHI_DX12_H_