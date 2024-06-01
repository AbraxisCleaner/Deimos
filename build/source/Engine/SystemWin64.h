#if !defined(_ENGINE_WIN64_SYSTEM_H_)
#define _ENGINE_WIN64_SYSTEM_H_

#include <pch.h>

#include <Windows.h>
#include <dxgi1_3.h>

struct SWin64State
{
	HINSTANCE hInstance;
	IDXGIFactory2 *pDxgiFactory;
	IDXGIAdapter1 *pDxgiAdapter;
	s64 ClockFreq;
};
extern SWin64State *gSystem;

namespace System
{
	bool Initialize();
	void Shutdown();

	s64 GetClock();
	f32 GetElapsedSeconds(s64 Start, s64 End);
}

#endif // _ENGINE_WIN64_SYSTEM_H_