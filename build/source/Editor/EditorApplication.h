#pragma once
#include <pch.h>
#include <Engine/RhiVulkan.h>

#include <Windows.h>

struct SEditorCreateInfo
{
	uint iWidth;
	uint iHeight;
	int iCmdLineFlags;
};

#define EDITOR_FLAG_FORCE_VK_VALIDATION 0x01
#define EDITOR_FLAG_ALLOW_GAME_CONSOLE 0x02

#define EDITOR_WND_QUIT 0x01
#define EDITOR_WND_NOFOCUS 0x02
#define EDITOR_WND_RESIZED 0x04

class CEditorApplication
{
public:
	bool Initialize(const SEditorCreateInfo &Info);
	void Release();
	void Tick();

	inline void ShowWindow() { ::ShowWindow(m_Hwnd, SW_SHOW); }
	inline void HideWindow() { ::ShowWindow(m_Hwnd, SW_HIDE); }
	inline bool ShouldClose() { return m_iHwndState & EDITOR_WND_QUIT; }

private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	HINSTANCE m_hInstance;
	int m_iCmdLineFlags;

	HWND m_Hwnd;
	int m_iHwndState;
	uint m_uHwndSize[2]; 
	RHI::SWindowContext m_MainWndContext;

	struct {
		int8 Kbd[256];
	} m_Input;

	bool m_bMainThreadInitialized;
};
extern CEditorApplication *gEditor;