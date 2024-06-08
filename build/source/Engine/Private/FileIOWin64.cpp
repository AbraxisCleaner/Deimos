#include <pch.h>
#include <Engine/FileIO.h>

#include <Windows.h>

bool ReadEntireFile(const TCHAR *Path, fileBlob_t *pBlob)
{
	ZeroThat(pBlob);
	
	HANDLE File = ::CreateFile(Path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 128, nullptr);
	if (File != (HANDLE)-1) {
		LARGE_INTEGER Li;
		::GetFileSizeEx(File, &Li);
		pBlob->Size = Li.QuadPart;

		pBlob->pBuffer = malloc(pBlob->Size);
		
		BOOL hr = ::ReadFile(File, pBlob->pBuffer, Li.LowPart, nullptr, nullptr);
		if (hr && Li.HighPart)
			::ReadFile(File, (void *)(((intptr_t)pBlob->pBuffer) + Li.LowPart), Li.HighPart, nullptr, nullptr);

		::CloseHandle(File);
		return true;
	}

	return false;
}

void ReleaseFileBlob(fileBlob_t *pBlob)
{
	if (pBlob->pBuffer) {
		free(pBlob->pBuffer);
		ZeroThat(pBlob);
	}
}

bool MapFile(const TCHAR *Path, mappedFile_t *pFile)
{
	pFile->Handle = (void *)::CreateFile(Path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 128, nullptr);
	if (pFile->Handle != (void *)(HANDLE)-1) {
		LARGE_INTEGER Li;
		::GetFileSizeEx((HANDLE)(pFile->Handle), &Li);
		pFile->Size = Li.QuadPart;

		HANDLE mapping = ::CreateFileMapping((HANDLE)pFile->Handle, nullptr, PAGE_READWRITE, 0, 0, nullptr);
		pFile->pView = ::MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, pFile->Size);

		return true;
	}
	return false;
}

void UnmapFile(mappedFile_t *pFile)
{
	if (pFile->pView) {
		::UnmapViewOfFile(pFile->pView);
		HANDLE mapping = ::CreateFileMappingW((HANDLE)pFile->Handle, nullptr, PAGE_READWRITE, 0, 0, nullptr);
		::CloseHandle(mapping);
		::CloseHandle((HANDLE)pFile->Handle);
		ZeroThat(pFile);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool LoadSharedLib(sharedLib_t *pLib, TString<TCHAR> Path, bool bCopyFile)
{
	ZeroThat(pLib);

	if (!bCopyFile) {
		pLib->fileName = TString<TCHAR>();
		pLib->handle = ::LoadLibrary(Path);
		if (pLib->handle)
			return true;
		return false;
	}

	pLib->fileName = Path;

	Path.Insert(STR("_LOCKED"), Path.GetFileExtension());
	CopyFile(pLib->fileName, Path, false);

	pLib->handle = ::LoadLibrary(Path);
	
	WIN32_FILE_ATTRIBUTE_DATA Fad = {};
	::GetFileAttributesEx((TCHAR *)pLib->fileName, GetFileExInfoStandard, &Fad);

	pLib->lastWrite = (size_t)Fad.ftLastWriteTime.dwHighDateTime << 32 | Fad.ftLastWriteTime.dwLowDateTime;

	return true;
}

void ReleaseSharedLib(sharedLib_t *pLib)
{
	::FreeLibrary((HMODULE)pLib->handle);
	if (pLib->lastWrite != 0) {
		pLib->fileName.Insert(STR("_LOCKED"), pLib->fileName.GetFileExtension());
		::DeleteFile((TCHAR *)pLib->fileName);
	}
	ZeroThat(pLib);
	pLib->fileName = TString<TCHAR>();
}

bool IsSharedLibDirty(sharedLib_t *pLib)
{
	if (pLib->lastWrite != 0) {
		WIN32_FILE_ATTRIBUTE_DATA Fad = {};
		::GetFileAttributesEx(pLib->fileName, GetFileExInfoStandard, &Fad);

		if (((size_t)Fad.ftLastWriteTime.dwHighDateTime << 32 | Fad.ftLastWriteTime.dwLowDateTime) > pLib->lastWrite) {
			return true;
		}
	}
	return false;
}

void *LoadProc(sharedLib_t pLib, const char *ProcName)
{
	return ::GetProcAddress((HMODULE)pLib.handle, ProcName);
}