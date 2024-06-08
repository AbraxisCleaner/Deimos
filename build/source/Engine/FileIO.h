#ifndef _ENGINE_FILESYSTEM_H_
#define _ENGINE_FILESYSTEM_H_

#include "Stl/String.h"

#ifdef PLATFORM_WIN64
#	undef LoadLibrary
#endif

struct fileBlob_t
{
	size_t Size;
	void *pBuffer;
};
bool ReadEntireFile(const TCHAR *Path, fileBlob_t *pBlob);
void ReleaseFileBlob(fileBlob_t *pBlob);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct mappedFile_t
{
	void *Handle;
	size_t Size;
	void *pView;

	void Release();
};
bool MapFile(const TCHAR *Path, mappedFile_t *pOut);
void UnmapFile(mappedFile_t *pFile);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Named after the Win32 name for a DLL because SDLL is too small and stinky.
struct sharedLib_t
{
	void *handle;
	size_t lastWrite;
	TString<TCHAR> fileName; // Name of the DLL that has been copied.
};
bool LoadSharedLib(sharedLib_t *pLib, TString<TCHAR> path, bool bCopyFile);
void ReleaseSharedLib(sharedLib_t *pLib);
bool IsSharedLibDirty(sharedLib_t *pLib);
void *LoadProc(sharedLib_t pLib, const char *procName);

#endif // _ENGINE_FILESYSTEM_H_