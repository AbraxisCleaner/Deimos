#ifndef _ENGINE_FILESYSTEM_H_
#define _ENGINE_FILESYSTEM_H_

#include "Stl/String.h"

#ifdef PLATFORM_WIN64
#	undef LoadLibrary
#endif

struct SFileBlob
{
	size_t Size;
	void *pBuffer;

	inline void Release() {
		if (pBuffer)
			free(pBuffer);
		ZeroThis();
	}
};
bool ReadEntireFile(const TCHAR *Path, SFileBlob *pBlob);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct SMappedFile
{
	void *Handle;
	size_t Size;
	void *pView;

	void Release();
};
bool MapFile(const TCHAR *Path, SMappedFile *pOut);
void UnmapFile(SMappedFile *pFile);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Named after the Win32 name for a DLL because SDLL is too small and stinky.
struct SSharedLib
{
	void *Handle;
	size_t LastWrite;
	TString<TCHAR> BaseFileName; // Name of the DLL that has been copied.
};
bool LoadSharedLib(SSharedLib *pLib, TString<TCHAR> Path, bool bCopyFile);
void ReleaseSharedLib(SSharedLib *pLib);
bool IsSharedLibDirty(SSharedLib *pLib);
void *LoadProc(SSharedLib pLib, const char *ProcName);

#endif // _ENGINE_FILESYSTEM_H_