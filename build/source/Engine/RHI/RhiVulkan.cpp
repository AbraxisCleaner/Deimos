#include "pch.h"
#include "Engine/DebugLog.h"
#include "Engine/FileIO.h"
#include "Engine/RhiVulkan.h"

#include <Windows.h>

RHI::SRhiState _Vk = {};
RHI::SRhiState *RHI::pVk = &_Vk;

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
#ifdef PLATFORM_WIN64
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

	// -- Create default command pool & buffer.
	VkCommandPoolCreateInfo cmdPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	cmdPoolInfo.queueFamilyIndex = pVk->QueueFamily;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	vkCreateCommandPool(pVk->Device, &cmdPoolInfo, nullptr, &pVk->CommandPool);

	VkCommandBufferAllocateInfo cmdBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	cmdBufferInfo.commandPool = pVk->CommandPool;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(pVk->Device, &cmdBufferInfo, &pVk->Cmd);

	// -- Load default shaders.
	VkShaderModule vertModule;
	VkShaderModule fragModule;

	SFileBlob vs, fs;
	ASSERT(ReadEntireFile(STR("data\\cso\\Static.spv"), &vs));
	ASSERT(ReadEntireFile(STR("data\\cso\\Illum.spv"), &fs));

	VkShaderModuleCreateInfo shaderModuleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	
	shaderModuleInfo.codeSize = vs.Size;
	shaderModuleInfo.pCode = (const uint32_t *)vs.pBuffer;
	hr = vkCreateShaderModule(pVk->Device, &shaderModuleInfo, nullptr, &vertModule);

	shaderModuleInfo.codeSize = fs.Size;
	shaderModuleInfo.pCode = (const uint32_t *)fs.pBuffer;
	hr = vkCreateShaderModule(pVk->Device, &shaderModuleInfo, nullptr, &fragModule);

	VkPipelineShaderStageCreateInfo shaderStages[2] = {
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO }
	};
	
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertModule;
	shaderStages[0].pName = "main";

	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragModule;
	shaderStages[1].pName = "main";

	// -- Fill in default pipline state info structs.
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

	VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo viewportInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportInfo.viewportCount = 1;
	viewportInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.lineWidth = 1.0f;
	
	VkPipelineColorBlendAttachmentState colorBlend = {};
	colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlend.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo blendInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	blendInfo.attachmentCount = 1;
	blendInfo.pAttachments = &colorBlend;
	blendInfo.logicOp = VK_LOGIC_OP_COPY;

	VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineLayoutCreateInfo pipelineLayout = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	vkCreatePipelineLayout(pVk->Device, &pipelineLayout, nullptr, &pVk->PipelineLayout);

	// -- Create default render passes.
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDependency subpassDependency = {};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &subpassDependency;

	hr = vkCreateRenderPass(pVk->Device, &renderPassInfo, nullptr, &pVk->RenderPass);
	ASSERT(hr == VK_SUCCESS);

	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &assemblyInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pColorBlendState = &blendInfo;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.layout = pVk->PipelineLayout;
	pipelineInfo.renderPass = pVk->RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineIndex = -1;

	hr = vkCreateGraphicsPipelines(pVk->Device, nullptr, 1, &pipelineInfo, nullptr, &pVk->Pipeline);
	ASSERT(hr == VK_SUCCESS);

	vkDestroyShaderModule(pVk->Device, fragModule, nullptr);
	vkDestroyShaderModule(pVk->Device, vertModule, nullptr);
	fs.Release();
	vs.Release();

	// -- Create Sync objects.
	VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore(pVk->Device, &semaphoreInfo, nullptr, &pVk->RenderSemaphore);
	
	VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(pVk->Device, &fenceInfo, nullptr, &pVk->QueueFence);

	return true;
}

void RHI::Release()
{
	vkDeviceWaitIdle(pVk->Device);
	vkDestroyFence(pVk->Device, pVk->QueueFence, nullptr);
	vkDestroySemaphore(pVk->Device, pVk->RenderSemaphore, nullptr);
	vkDestroyPipeline(pVk->Device, pVk->Pipeline, nullptr);
	vkDestroyRenderPass(pVk->Device, pVk->RenderPass, nullptr);
	vkDestroyPipelineLayout(pVk->Device, pVk->PipelineLayout, nullptr);
	vkDestroyCommandPool(pVk->Device, pVk->CommandPool, nullptr);
	vkDestroyDevice(pVk->Device, nullptr);
	vkDestroyDebugUtilsMessengerEXT(pVk->Instance, pVk->DebugMessenger, nullptr);
	vkDestroyInstance(pVk->Instance, nullptr);
	ZeroThat(pVk);
}

void RHI::WaitForRendering()
{
	vkWaitForFences(pVk->Device, 1, &pVk->QueueFence, VK_TRUE, (uint64)-1);
	vkResetFences(pVk->Device, 1, &pVk->QueueFence);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool RHI::CreateWindowContext(void *pWnd, SWindowContext *pCtx)
{
	HWND hwnd = (HWND)pWnd;

#ifdef PLATFORM_WIN64
	VkWin32SurfaceCreateInfoKHR surfaceInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	surfaceInfo.hinstance = ::GetModuleHandle(nullptr);
	surfaceInfo.hwnd = hwnd;

	vkCreateWin32SurfaceKHR(pVk->Instance, &surfaceInfo, nullptr, &pCtx->Surface);
#endif

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pVk->Adapter, pCtx->Surface, &surfaceCapabilities);

	ZeroThat(&pCtx->SwapchainInfo);
	pCtx->SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	pCtx->SwapchainInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	pCtx->SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	pCtx->SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	pCtx->SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	pCtx->SwapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	pCtx->SwapchainInfo.minImageCount = 2;
	pCtx->SwapchainInfo.clipped = false;
	pCtx->SwapchainInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	pCtx->SwapchainInfo.imageArrayLayers = 1;
	pCtx->SwapchainInfo.surface = pCtx->Surface;
	pCtx->SwapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
	pCtx->SwapchainInfo.preTransform = surfaceCapabilities.currentTransform;
	
	auto hr = vkCreateSwapchainKHR(pVk->Device, &pCtx->SwapchainInfo, nullptr, &pCtx->Swapchain);
	if (hr != VK_SUCCESS) {
		vkDestroySurfaceKHR(pVk->Instance, pCtx->Surface, nullptr);
		return false;
	}

	// -- Retrieve images.
	VkImage images[2];
	vkGetSwapchainImagesKHR(pVk->Device, pCtx->Swapchain, &pCtx->SwapchainInfo.minImageCount, images);

	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.format = pCtx->SwapchainInfo.imageFormat;
	viewInfo.image = images[0];
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	
	vkCreateImageView(pVk->Device, &viewInfo, nullptr, &pCtx->ImageViews[0]);
	viewInfo.image = images[1];
	vkCreateImageView(pVk->Device, &viewInfo, nullptr, &pCtx->ImageViews[1]);

	// -- Create Framebuffers
	VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	framebufferInfo.renderPass = pVk->RenderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.width = pCtx->SwapchainInfo.imageExtent.width;
	framebufferInfo.height = pCtx->SwapchainInfo.imageExtent.height;
	framebufferInfo.layers = 1;

	framebufferInfo.pAttachments = &pCtx->ImageViews[0];
	vkCreateFramebuffer(pVk->Device, &framebufferInfo, nullptr, &pCtx->Framebuffers[0]);

	framebufferInfo.pAttachments = &pCtx->ImageViews[1];
	vkCreateFramebuffer(pVk->Device, &framebufferInfo, nullptr, &pCtx->Framebuffers[1]);

	// -- Create semaphore
	VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore(pVk->Device, &semaphoreInfo, nullptr, &pCtx->ImageSemaphore);

	return true;
}

void RHI::ReleaseWindowContext(SWindowContext *pCtx)
{
	RHI::WaitForRendering();
	vkDestroySemaphore(pVk->Device, pCtx->ImageSemaphore, nullptr);
	vkDestroyFramebuffer(pVk->Device, pCtx->Framebuffers[1], nullptr);
	vkDestroyFramebuffer(pVk->Device, pCtx->Framebuffers[0], nullptr);
	vkDestroyImageView(pVk->Device, pCtx->ImageViews[1], nullptr);
	vkDestroyImageView(pVk->Device, pCtx->ImageViews[0], nullptr);
	vkDestroySwapchainKHR(pVk->Device, pCtx->Swapchain, nullptr);
	vkDestroySurfaceKHR(pVk->Instance, pCtx->Surface, nullptr);
	ZeroThat(pCtx);
}

void RHI::ResizeWindowContext(SWindowContext *pCtx, uint NewWidth, uint NewHeight)
{
	RHI::WaitForRendering();

	vkDestroyFramebuffer(pVk->Device, pCtx->Framebuffers[1], nullptr);
	vkDestroyFramebuffer(pVk->Device, pCtx->Framebuffers[0], nullptr);
	vkDestroyImageView(pVk->Device, pCtx->ImageViews[1], nullptr);
	vkDestroyImageView(pVk->Device, pCtx->ImageViews[0], nullptr);
	
	vkDestroySwapchainKHR(pVk->Device, pCtx->Swapchain, nullptr);

	VkSurfaceCapabilitiesKHR surfaceCaps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pVk->Adapter, pCtx->Surface, &surfaceCaps);

	if (NewWidth != 0)
		pCtx->SwapchainInfo.imageExtent.width = NewWidth;
	else
		pCtx->SwapchainInfo.imageExtent.width = surfaceCaps.currentExtent.width;

	if (NewHeight != 0)
		pCtx->SwapchainInfo.imageExtent.height = NewHeight;
	else
		pCtx->SwapchainInfo.imageExtent.height = surfaceCaps.currentExtent.height;

	pCtx->SwapchainInfo.preTransform = surfaceCaps.currentTransform;

	vkCreateSwapchainKHR(pVk->Device, &pCtx->SwapchainInfo, nullptr, &pCtx->Swapchain);

	VkImage images[2];
	vkGetSwapchainImagesKHR(pVk->Device, pCtx->Swapchain, &pCtx->SwapchainInfo.minImageCount, images);

	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.format = pCtx->SwapchainInfo.imageFormat;
	viewInfo.image = images[0];
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	vkCreateImageView(pVk->Device, &viewInfo, nullptr, &pCtx->ImageViews[0]);
	viewInfo.image = images[1];
	vkCreateImageView(pVk->Device, &viewInfo, nullptr, &pCtx->ImageViews[1]);

	// -- Create Framebuffers
	VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	framebufferInfo.renderPass = pVk->RenderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.width = pCtx->SwapchainInfo.imageExtent.width;
	framebufferInfo.height = pCtx->SwapchainInfo.imageExtent.height;
	framebufferInfo.layers = 1;

	framebufferInfo.pAttachments = &pCtx->ImageViews[0];
	vkCreateFramebuffer(pVk->Device, &framebufferInfo, nullptr, &pCtx->Framebuffers[0]);

	framebufferInfo.pAttachments = &pCtx->ImageViews[1];
	vkCreateFramebuffer(pVk->Device, &framebufferInfo, nullptr, &pCtx->Framebuffers[1]);
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
