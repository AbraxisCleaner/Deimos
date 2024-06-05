#include "pch.h"
#include "FileIO.h"
#include <Windows.h>

bool ReadEntireFile(const TCHAR *Path, SFileBlob *pBlob)
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

bool MapFile(const TCHAR *Path, SMappedFile *pFile)
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

void UnmapFile(SMappedFile *pFile)
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
bool LoadSharedLib(SSharedLib *pLib, TString<TCHAR> Path, bool bCopyFile)
{
	ZeroThat(pLib);

	if (!bCopyFile) {
		pLib->BaseFileName = TString<TCHAR>();
		pLib->Handle = ::LoadLibrary(Path);
		if (pLib->Handle)
			return true;
		return false;
	}

	pLib->BaseFileName = Path;

	Path.Insert(STR("_LOCKED"), Path.GetFileExtension());
	CopyFile(pLib->BaseFileName, Path, false);

	pLib->Handle = ::LoadLibrary(Path);
	
	WIN32_FILE_ATTRIBUTE_DATA Fad = {};
	::GetFileAttributesEx((TCHAR *)pLib->BaseFileName, GetFileExInfoStandard, &Fad);

	pLib->LastWrite = (size_t)Fad.ftLastWriteTime.dwHighDateTime << 32 | Fad.ftLastWriteTime.dwLowDateTime;

	return true;
}

void ReleaseSharedLib(SSharedLib *pLib)
{
	::FreeLibrary((HMODULE)pLib->Handle);
	if (pLib->LastWrite != 0) {
		pLib->BaseFileName.Insert(STR("_LOCKED"), pLib->BaseFileName.GetFileExtension());
		::DeleteFile((TCHAR *)pLib->BaseFileName);
	}
	ZeroThat(pLib);
	pLib->BaseFileName = TString<TCHAR>();
}

bool IsSharedLibDirty(SSharedLib *pLib)
{
	if (pLib->LastWrite != 0) {
		WIN32_FILE_ATTRIBUTE_DATA Fad = {};
		::GetFileAttributesEx(pLib->BaseFileName, GetFileExInfoStandard, &Fad);

		if (((size_t)Fad.ftLastWriteTime.dwHighDateTime << 32 | Fad.ftLastWriteTime.dwLowDateTime) > pLib->LastWrite) {
			return true;
		}
	}
	return false;
}

void *LoadProc(SSharedLib pLib, const char *ProcName)
{
	return ::GetProcAddress((HMODULE)pLib.Handle, ProcName);
}