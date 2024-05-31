#include <pch.h>
#include <Engine/Direct3D12.h>

Engine::IRhiD3D12 _d3d;
Engine::IRhiD3D12 *gd3d = &_d3d;

Engine::IRhiD3D12::IRhiD3D12()
{
#if defined(_DEBUG)
	ID3D12Debug *pDebug1;
	::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug1));
	pDebug1->EnableDebugLayer();
	pDebug1->Release();
#endif
	
	pDevice = NULL;

	if (FAILED(::CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory)))) {
		return;
	}
	pDxgiFactory->EnumAdapters1(0, &pDxgiAdapter);

	if (FAILED(::D3D12CreateDevice(pDxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice)))) {
		pDxgiAdapter->Release();
		pDxgiFactory->Release();
		return;
	}

	D3D12_COMMAND_QUEUE_DESC CmdQueueDesc = { .Type = D3D12_COMMAND_LIST_TYPE_DIRECT };
	pDevice->CreateCommandQueue(&CmdQueueDesc, IID_PPV_ARGS(&pCmdQueue));
	pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
	FenceEvent = ::CreateEventW(NULL, false, false, NULL);
	FenceValue = 0;

	RtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	GenericDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCmdAllocator));
	pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCmdAllocator, NULL, IID_PPV_ARGS(&pCmd));
	pCmd->Close();
}

Engine::IRhiD3D12::~IRhiD3D12()
{
	if (pDevice) {
		WaitForGpu();
		pCmd->Release();
		pCmdAllocator->Release();
		::CloseHandle(FenceEvent);
		pFence->Release();
		pCmdQueue->Release();
		pDevice->Release();
		pDxgiAdapter->Release();
		pDxgiFactory->Release();
		ZeroThis();
	}
}

void Engine::IRhiD3D12::WaitForGpu()
{
	if (pFence->GetCompletedValue() != FenceValue) {
		pFence->SetEventOnCompletion(FenceValue, FenceEvent);
		::WaitForSingleObject(FenceEvent, INFINITE);
	}
}