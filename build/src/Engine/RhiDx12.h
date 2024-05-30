#if !defined(_ENGINE_RHI_DX12_H_)
#define _ENGINE_RHI_DX12_H_

#include <pch.h>

#include <dxgi1_4.h>
#include <d3d12.h>

namespace Engine::Rhi
{
    class IRhiD3D12 {
        IDXGIFactory2 *m_pDxgiFactory;
        IDXGIAdapter1 *m_pDxgiAdapter;
        ID3D12Device *m_pDevice;
        ID3D12CommandQueue *m_pCmdQueue;
        ID3D12Fence *m_pFence;
        HANDLE m_FenceEvent;
        uint m_FenceValue;
        uint m_RtvDescriptorSize;
        uint m_GenericDescriptorSize;
        ID3D12CommandAllocator *m_pCmdAllocator;
        ID3D12GraphicsCommandList *m_pCmd;

    public:
        bool Initialize();
        void Release();
        
        void NewFrame();
        void EndFrame();

        void WaitForDrawing();

        inline ID3D12GraphicsCommandList *GetCommandList() { return m_pCmd; }
    };
}

#endif // _ENGINE_RHI_DX12_H_