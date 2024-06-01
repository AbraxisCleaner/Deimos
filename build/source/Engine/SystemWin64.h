#if !defined(_ENGINE_WIN64_SYSTEM_H_)
#define _ENGINE_WIN64_SYSTEM_H_

#include <pch.h>

#include <Windows.h>
#include <dxgi1_3.h>

namespace System
{
	struct SWin64State
	{
		HINSTANCE hInstance;
		IDXGIFactory2 *pDxgiFactory;
		IDXGIAdapter1 *pDxgiAdapter;
		s64 ClockFreq;
	};
	extern SWin64State *gWin64;

	namespace Win64
	{
		bool Initialize();
		void Shutdown();
	}

	static inline s64 GetClock() {
		LARGE_INTEGER Li;
		::QueryPerformanceCounter(&Li);
		return Li.QuadPart;
	}

	static inline f32 GetElapsedSeconds(s64 Start, s64 End) {
		return ((f32)(End - Start) / (f32)gWin64->ClockFreq);
	}
}

#endif // _ENGINE_WIN64_SYSTEM_H_