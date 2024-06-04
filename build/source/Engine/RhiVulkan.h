#pragma once
#include <pch.h>
#include <Stl/Container.h>

#include <volk.h>

namespace RHI {
	void PrintAvailableVulkanLayers();            
	void PrintAvailableVulkanInstanceExtensions();
	void PrintAvailableVulkanDeviceExtensions(VkPhysicalDevice Adapter);

	bool CheckVulkanLayerPresent(uint32_t Count, ...);
	bool CheckVulkanInstanceExtensionPresent(uint32_t Count, ...);
	bool CheckVulkanDeviceExtensionPresent(VkPhysicalDevice Adapter, uint32_t Count, ...);

	struct SWindowContext
	{
		VkSurfaceKHR Surface;
		VkSwapchainCreateInfoKHR SwapchainInfo;
		VkSwapchainKHR Swapchain;
		VkImageView ImageViews[2];
		VkFramebuffer Framebuffers[2];
		VkSemaphore ImageSemaphore;
	};
	bool CreateWindowContext(HWND hwnd, SWindowContext *pCtx);
	void ReleaseWindowContext(SWindowContext *pCtx);
	void ResizeWindowContext(SWindowContext *pCtx, uint NewWidth, uint NewHeight);

	struct SRhiState
	{
		VkAllocationCallbacks Allocator;
		bool bValidate;

		VkInstance Instance;
		VkDebugUtilsMessengerEXT DebugMessenger;
		VkPhysicalDevice Adapter;
		VkDevice Device;
		VkQueue MainQueue;
		uint QueueFamily;
		VkFence QueueFence;
		VkSemaphore RenderSemaphore;

		VkRenderPass RenderPass;
		VkPipelineLayout PipelineLayout;
		VkPipeline Pipeline;

		VkCommandPool CommandPool;
		VkCommandBuffer Cmd;
	};
	extern SRhiState *pVk;

	bool Initialize(bool EnableDebugging);
	void Release();

	void WaitForRendering();

	inline void SetRhiState(SRhiState *pState) { pVk = pState; }
	inline SRhiState *GetRhiState() { return pVk; }
}