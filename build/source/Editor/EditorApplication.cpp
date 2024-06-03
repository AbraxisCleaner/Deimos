#include <pch.h>
#include "Engine/DebugLog.h"
#include "EditorApplication.h"
#include "Engine/InputKey.inl"
#include "Engine/RhiVulkan.h"

CEditorApplication *gEditor = nullptr;

void CEditorApplication::Tick()
{
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (m_Input.Kbd[eKey_Escape])
		m_iHwndState |= EDITOR_WND_QUIT;
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