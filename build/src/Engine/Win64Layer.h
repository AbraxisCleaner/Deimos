#if !defined(_ENGINE_WIN64_LAYER_H_)
#define _ENGINE_WIN64_LAYER_H_

#include <pch.h>
#include <Engine/Arena.h>

#include <Windows.h>


namespace Engine::Win64
{
	struct SWin64PersistentData
	{
		HINSTANCE hInstance;
		CStackArena TransientArena;
	};
	extern SWin64PersistentData *gWin64;

	extern bool InitializeLayer(const TCHAR *szCmdLine);
	extern void ShutdownLayer();

	static inline void *TempMalloc(size_t Size) {
		return gWin64->TransientArena.Alloc(Size);
	}

	template <typename T> T *TempMallocT(uint Count) {
		T *pDst = static_cast<T *>(gWin64->TransientArena.Alloc(Count * sizeof(T)));
		return pDst;
	}
}

#endif // _ENGINE_WIN64_LAYER_H_