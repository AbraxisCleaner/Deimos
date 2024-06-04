#include <pch.h>
#include <Engine/DebugLog.h>

#include "RhiVulkan.h"

#include <Windows.h>

// NOTE: Filmic Tonemapping + Hable's Curve


RHI::SVulkanState _Vk = {};
RHI::SVulkanState *RHI::pVk = &_Vk;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
VkBool32 VulkanDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
										VkDebugUtilsMessageTypeFlagsEXT messageTypes,
										const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
										void *pUserData)
{
	const char *sType = "";
	switch (messageTypes) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: sType = "VALIDATION"; break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: sType = "PERFORMANCE"; break;
	}

	const char *sSeverity = "";
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: sSeverity = "ERROR"; break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: sSeverity = "WARNING"; break;
	}

	printf("[VK_%s_%s] :: %s\n", sType, sSeverity, pCallbackData->pMessage);
	return VK_FALSE;
}

bool RHI::Initialize(bool EnableDebugging)
{
	volkInitialize();

	const uint numInstanceLayers = 1;
	const char *instanceLayers[numInstanceLayers] = {
		"VK_LAYER_KHRONOS_validation",
	};

	// -- Create instance
	const uint numInstanceExtensions = 3;
	const char *instanceExtensions[numInstanceExtensions] = {
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
		"VK_EXT_debug_utils",
	};

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messengerInfo.pfnUserCallback = VulkanDebugMessengerCallback;

	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pApplicationName = "Editor";
	appInfo.pEngineName = "Talos";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = numInstanceExtensions;
	instanceInfo.ppEnabledExtensionNames = instanceExtensions;
#if _DEBUG
	if (EnableDebugging) {
		instanceInfo.pNext = &messengerInfo;
		instanceInfo.enabledLayerCount = numInstanceLayers;
		instanceInfo.ppEnabledLayerNames = instanceLayers;
	}
#endif

	auto hr = vkCreateInstance(&instanceInfo, nullptr, &pVk->Instance);
	if (hr != VK_SUCCESS) {
		switch (hr) {
		case VK_ERROR_EXTENSION_NOT_PRESENT: printf("[vkCreateaInstance] :: Extension not present.\n"); break;
		case VK_ERROR_LAYER_NOT_PRESENT: printf("[vkCreateaInstance] :: Layer not present.\n"); break;
		case VK_ERROR_INITIALIZATION_FAILED: printf("[vkCreateaInstance] :: Initialization failed.\n"); break;
		default: printf("[vkCreateInstance] :: Unknown error.\n"); break;
		}
		return false;
	}

	volkLoadInstanceOnly(pVk->Instance);

#ifdef _DEBUG
	if (EnableDebugging) {
		vkCreateDebugUtilsMessengerEXT(pVk->Instance, &messengerInfo, nullptr, &pVk->DebugMessenger);
	}
#endif

	// -- Choose adapter;
	uint numAdapters;
	vkEnumeratePhysicalDevices(pVk->Instance, &numAdapters, nullptr);
	auto pAdapters = (VkPhysicalDevice *)malloc(numAdapters * sizeof(intptr_t));
	vkEnumeratePhysicalDevices(pVk->Instance, &numAdapters, pAdapters);

	for (auto i = 0; i != numAdapters; ++i) {
		VkPhysicalDeviceProperties props;
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceProperties(pAdapters[i], &props);
		vkGetPhysicalDeviceFeatures(pAdapters[i], &features);

		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
			&& features.geometryShader) 
		{
			pVk->Adapter = pAdapters[i];
		}
	}

	free(pAdapters);
	ASSERT(pVk->Adapter);

	// -- Get device queues.
	pVk->QueueFamily = (uint)-1;

	uint numQueueFamilies;
	vkGetPhysicalDeviceQueueFamilyProperties(pVk->Adapter, &numQueueFamilies, nullptr);
	auto pQueueFamilies = (VkQueueFamilyProperties *)malloc(numQueueFamilies * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(pVk->Adapter, &numQueueFamilies, pQueueFamilies);

	for (auto i = 0; i != numQueueFamilies; ++i) {
		auto _GetPhysicalDeviceWin32PresentationSupport = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(pVk->Instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
		ASSERT(_GetPhysicalDeviceWin32PresentationSupport);
#ifdef ENGINE_WIN64
		VkBool32 presentationSupport = _GetPhysicalDeviceWin32PresentationSupport(pVk->Adapter, i);
#endif

		if (pQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentationSupport)
			pVk->QueueFamily = i;
	}

	free(pQueueFamilies);

	// -- Create device.
	const uint numRequiredDeviceExtensions = 1;
	const char *requiredDeviceExtensions[numRequiredDeviceExtensions] = {
		"VK_KHR_swapchain",
	};

	float queuep = 1.0f;

	VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueInfo.pQueuePriorities = &queuep;
	queueInfo.queueCount = 1;
	queueInfo.queueFamilyIndex = pVk->QueueFamily;
	
	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.enabledExtensionCount = numRequiredDeviceExtensions;
	deviceInfo.ppEnabledExtensionNames = requiredDeviceExtensions;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;
#ifdef _DEBUG
	if (EnableDebugging) {
		deviceInfo.ppEnabledLayerNames = instanceLayers;
		deviceInfo.enabledLayerCount = numInstanceLayers;
	}
#endif

	hr = vkCreateDevice(pVk->Adapter, &deviceInfo, nullptr, &pVk->Device);
	if (hr != VK_SUCCESS) {
		return false;
	}

	volkLoadDevice(pVk->Device);
	vkGetDeviceQueue(pVk->Device, pVk->QueueFamily, 0, &pVk->MainQueue);
	return true;
}

void RHI::Release()
{
	vkDestroyDevice(pVk->Device, nullptr);
	vkDestroyDebugUtilsMessengerEXT(pVk->Instance, pVk->DebugMessenger, nullptr);
	vkDestroyInstance(pVk->Instance, nullptr);
	ZeroThat(pVk);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool RHI::CreateWindowContext(HWND hwnd, SWindowContext *pCtx)
{
#ifdef ENGINE_WIN64
	VkWin32SurfaceCreateInfoKHR surfaceInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	surfaceInfo.hinstance = ::GetModuleHandle(nullptr);
	surfaceInfo.hwnd = hwnd;

	vkCreateWin32SurfaceKHR(pVk->Instance, &surfaceInfo, nullptr, &pCtx->Surface);
#endif

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pVk->Adapter, pCtx->Surface, &surfaceCapabilities);

	VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	swapchainInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainInfo.minImageCount = 2;
	swapchainInfo.clipped = false;
	swapchainInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.surface = pCtx->Surface;
	swapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
	swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
	
	auto hr = vkCreateSwapchainKHR(pVk->Device, &swapchainInfo, nullptr, &pCtx->Swapchain);
	if (hr != VK_SUCCESS) {
		vkDestroySurfaceKHR(pVk->Instance, pCtx->Surface, nullptr);
		return false;
	}

	VkImage images[2];
	vkGetSwapchainImagesKHR(pVk->Device, pCtx->Swapchain, &swapchainInfo.minImageCount, images);

	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.format = swapchainInfo.imageFormat;
	viewInfo.image = images[0];
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	
	vkCreateImageView(pVk->Device, &viewInfo, nullptr, &pCtx->ImageViews[0]);
	viewInfo.image = images[1];
	vkCreateImageView(pVk->Device, &viewInfo, nullptr, &pCtx->ImageViews[1]);

	return true;
}

void RHI::ReleaseWindowContext(SWindowContext *pCtx)
{
	vkDestroyImageView(pVk->Device, pCtx->ImageViews[1], nullptr);
	vkDestroyImageView(pVk->Device, pCtx->ImageViews[0], nullptr);
	vkDestroySwapchainKHR(pVk->Device, pCtx->Swapchain, nullptr);
	vkDestroySurfaceKHR(pVk->Instance, pCtx->Surface, nullptr);
	ZeroThat(pCtx);
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RHI::PrintAvailableVulkanLayers() {
	uint num;
	vkEnumerateInstanceLayerProperties(&num, nullptr);
	auto props = (VkLayerProperties *)malloc(num * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&num, props);

	printf("----- Vulkan Layers -----\n");
	for (auto i = 0; i != num; ++i) {
		printf("\t%s\n", props[i].layerName);
	}

	free(props);
}

void RHI::PrintAvailableVulkanInstanceExtensions() {
	uint num;
	vkEnumerateInstanceExtensionProperties(nullptr, &num, nullptr);
	auto props = (VkExtensionProperties *)malloc(num * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(nullptr, &num, props);

	printf("----- Vulkan Instance Extensions -----\n");
	for (auto i = 0; i != num; ++i) {
		printf("\t%s\n", props[i].extensionName);
	}

	free(props);
}

void RHI::PrintAvailableVulkanDeviceExtensions(VkPhysicalDevice Adapter) {
	uint num;
	vkEnumerateDeviceExtensionProperties(Adapter, nullptr, &num, nullptr);
	auto props = (VkExtensionProperties *)malloc(num * sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(Adapter, nullptr, &num, props);

	printf("----- Vulkan Device Extensions -----\n");
	for (auto i = 0; i != num; ++i) {
		printf("\t%s\n", props[i].extensionName);
	}

	free(props);
}

bool CheckVulkanLayerPresent(uint32_t Count, ...) {
	uint num;
	vkEnumerateInstanceLayerProperties(&num, nullptr);
	auto props = (VkLayerProperties *)malloc(num * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&num, props);

	const char *name = (const char *)&Count;
	++name;

	uint found = 0;
	bool next = false;
	for (auto i = 0; i != Count; ++i) {
		for (auto x = 0; x != num; ++x) {
			if (!strcmp(name, props[x].layerName)) {
				++found;
				next = true;
			}
			if (next) {
				next = false;
				break;
			}
		}
	}

	free(props);
	return (found == Count) ? true : false;
}

bool CheckVulkanInstanceExtensionPresent(uint32_t Count, ...) {
	uint num;
	vkEnumerateInstanceExtensionProperties(nullptr, &num, nullptr);
	auto props = (VkExtensionProperties *)malloc(num * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(nullptr, &num, props);

	const char *name = (const char *)&Count;
	++name;

	uint found = 0;
	bool next = false;
	for (auto i = 0; i != Count; ++i) {
		for (auto x = 0; x != num; ++x) {
			if (!strcmp(name, props[x].extensionName)) {
				++found;
				next = true;
			}
			if (next) {
				next = false;
				break;
			}
		}
	}

	free(props);
	return (found == Count) ? true : false;
}

bool CheckVulkanDeviceExtensionPresent(VkPhysicalDevice Adapter, uint32_t Count, ...) {
	uint num;
	vkEnumerateDeviceExtensionProperties(Adapter, nullptr, &num, nullptr);
	auto props = (VkExtensionProperties *)malloc(num * sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(Adapter, nullptr, &num, props);

	const char *name = (const char *)&Count;
	++name;

	uint found = 0;
	bool next = false;
	for (auto i = 0; i != Count; ++i) {
		for (auto x = 0; x != num; ++x) {
			if (!strcmp(name, props[x].extensionName)) {
				++found;
				next = true;
			}
			if (next) {
				next = false;
				break;
			}
		}
	}

	free(props);
	return (found == Count) ? true : false;
}
