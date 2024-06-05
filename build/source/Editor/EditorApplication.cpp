#include <pch.h>
#include "Engine/DebugLog.h"
#include "EditorApplication.h"
#include "Engine/InputKey.inl"
#include "Engine/Rhi/RhiVulkan.h"

CEditorApplication *gEditor = nullptr;

void CEditorApplication::Tick()
{
	m_iHwndState &= ~EDITOR_WND_RESIZED; // Clear the resized flag.

	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (m_Input.Kbd[eKey_Escape])
		m_iHwndState |= EDITOR_WND_QUIT;

	if (m_iHwndState & EDITOR_WND_RESIZED) {
		RHI::ResizeWindowContext(&m_MainWndContext, 0, 0);
	}

	// -- Rendering
	RHI::WaitForRendering();

	uint32_t imageIndex;
	vkAcquireNextImageKHR(RHI::gRhi->Device, m_MainWndContext.Swapchain, 0, m_MainWndContext.ImageSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkCommandBufferBeginInfo cmdBegin = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	vkResetCommandBuffer(RHI::gRhi->Cmd, 0);
	vkBeginCommandBuffer(RHI::gRhi->Cmd, &cmdBegin);

	// RenderPass
	VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

	VkRenderPassBeginInfo renderPass = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	renderPass.renderPass = RHI::gRhi->RenderPass;
	renderPass.framebuffer = m_MainWndContext.Framebuffers[imageIndex];
	renderPass.renderArea.extent = m_MainWndContext.SwapchainInfo.imageExtent;
	renderPass.pClearValues = &clearColor;
	renderPass.clearValueCount = 1;
	
	vkCmdBeginRenderPass(RHI::gRhi->Cmd, &renderPass, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(RHI::gRhi->Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, RHI::gRhi->Pipeline);

	VkViewport viewport = {};
	viewport.width = (float)m_uHwndSize[0];
	viewport.height = (float)m_uHwndSize[1];
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(RHI::gRhi->Cmd, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.extent = m_MainWndContext.SwapchainInfo.imageExtent;
	vkCmdSetScissor(RHI::gRhi->Cmd, 0, 1, &scissor);

	vkCmdDraw(RHI::gRhi->Cmd, 3, 1, 0, 0);

	vkCmdEndRenderPass(RHI::gRhi->Cmd);
	vkEndCommandBuffer(RHI::gRhi->Cmd);

	// Submit
	VkPipelineStageFlags stagesToWait[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_MainWndContext.ImageSemaphore;
	submitInfo.pWaitDstStageMask = stagesToWait;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &RHI::gRhi->Cmd;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &RHI::gRhi->RenderSemaphore;

	vkQueueSubmit(RHI::gRhi->MainQueue, 1, &submitInfo, RHI::gRhi->QueueFence);

	// Present
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &RHI::gRhi->RenderSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_MainWndContext.Swapchain;
	presentInfo.pImageIndices = &imageIndex;
	
	vkQueuePresentKHR(RHI::gRhi->MainQueue, &presentInfo);
}

LRESULT CALLBACK CEditorApplication::WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	if (gEditor->m_bMainThreadInitialized) {
		switch (umsg) {

		case WM_CLOSE: {
			gEditor->m_iHwndState |= EDITOR_WND_QUIT;
			return 0;
		} break;

		case WM_SIZE: {
			gEditor->m_iHwndState |= EDITOR_WND_RESIZED;

			RECT Rect;
			::GetClientRect(hwnd, &Rect);
			gEditor->m_uHwndSize[0] = (Rect.right - Rect.left);
			gEditor->m_uHwndSize[1] = (Rect.bottom - Rect.top);
		} break;

		case WM_INPUT: {
			RAWINPUT Raw;
			uint dwSize = sizeof(Raw);
			::GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &Raw, &dwSize, sizeof(RAWINPUTHEADER));

			switch (Raw.header.dwType) {
			case RIM_TYPEKEYBOARD: {
				switch (Raw.data.keyboard.Message) {
				case WM_KEYDOWN: gEditor->m_Input.Kbd[Raw.data.keyboard.VKey] = true; break;
				case WM_KEYUP: gEditor->m_Input.Kbd[Raw.data.keyboard.VKey] = false; break;
				} break;
			} break;

			case RIM_TYPEMOUSE: {
			} break;

			case RIM_TYPEHID: {
			} break;
			}
		} break;

		}
	}
	return ::DefWindowProcA(hwnd, umsg, wparam, lparam);
}

bool CEditorApplication::Initialize(const SEditorCreateInfo &Info)
{
	m_hInstance = ::GetModuleHandleA(nullptr);

	// -- Create Window
	WNDCLASSEXA WndClass = {};
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.hInstance = m_hInstance;
	WndClass.hCursor = ::LoadCursor(m_hInstance, IDC_ARROW);
	WndClass.lpfnWndProc = CEditorApplication::WndProc;
	WndClass.lpszClassName = "EditorApp";
	WndClass.style = CS_VREDRAW | CS_HREDRAW;
	::RegisterClassExA(&WndClass);

	MONITORINFO Monitor = { sizeof(Monitor) };
	::GetMonitorInfo(::MonitorFromPoint({}, 1), &Monitor);

	RECT Rect = { 0, 0, (int)Info.iWidth, (int)Info.iHeight };
	::AdjustWindowRect(&Rect, WS_TILEDWINDOW, FALSE);

	auto Px = ((Monitor.rcWork.right / 2) - (Rect.right / 2));
	auto Py = ((Monitor.rcWork.bottom / 2) - (Rect.bottom / 2));

	m_Hwnd = ::CreateWindowExA(0, WndClass.lpszClassName, "Editor", WS_TILEDWINDOW, Px, Py, Rect.right, Rect.bottom, nullptr, nullptr, m_hInstance, nullptr);
	ASSERT(m_Hwnd);

	::GetClientRect(m_Hwnd, &Rect);
	m_uHwndSize[0] = (Rect.right - Rect.left);
	m_uHwndSize[1] = (Rect.bottom - Rect.top);

	m_iHwndState = 0;
	
	// -- Register Input.
	memset(&m_Input, 0, sizeof(m_Input));

	RAWINPUTDEVICE Rid = {};
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x06;
	Rid.hwndTarget = m_Hwnd;
	Rid.dwFlags = RIDEV_NOLEGACY;

	::RegisterRawInputDevices(&Rid, 1, sizeof(Rid));

	// -- Initialize RHI Driver.
	RHI::Initialize(true); // @TODO: Make this actually care about whether or not we want to debug.
	RHI::CreateWindowContext(m_Hwnd, &m_MainWndContext);

	return true;
}

void CEditorApplication::Release()
{
	RHI::ReleaseWindowContext(&m_MainWndContext);
	RHI::Release();
	::DestroyWindow(m_Hwnd);
	ZeroThat(gEditor);
}