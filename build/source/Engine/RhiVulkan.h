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
		VkSwapchainKHR Swapchain;
		VkImageView ImageViews[2];
	};
	bool CreateWindowContext(HWND hwnd, SWindowContext *pCtx);
	void ReleaseWindowContext(SWindowContext *pCtx);

	struct SVulkanState
	{
		VkAllocationCallbacks Allocator;
		bool Debugging;

		VkInstance Instance;
		VkDebugUtilsMessengerEXT DebugMessenger;
		VkPhysicalDevice Adapter;
		VkDevice Device;
		VkQueue MainQueue;
		uint QueueFamily;
	};
	extern SVulkanState *pVk;

	bool Initialize(bool EnableDebugging);
	void Release();

	inline void SetVulkanState(SVulkanState *pState) { pVk = pState; }
	inline SVulkanState *GetVulkanState() { return pVk; }
}