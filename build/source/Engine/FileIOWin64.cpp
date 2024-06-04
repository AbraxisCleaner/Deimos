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