#include <pch.h>
#include <Engine/SystemWin64.h>
#include <Engine/FileSystem.h>
#include <Engine/DebugLog.h>

#define STB_SPRINTF_IMPLEMENTATION
#include <../vendor/stb_sprintf.h>

#if defined(_DEBUG)
#	include <dxgidebug.h>
#endif

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
SWin64State _Win64 = {};
SWin64State *gSystem= &_Win64;

bool System::Initialize()
{
	gSystem->hInstance = ::GetModuleHandleA(nullptr);
	
	::CreateDXGIFactory1(IID_PPV_ARGS(&gSystem->pDxgiFactory));
	gSystem->pDxgiFactory->EnumAdapters1(0, &gSystem->pDxgiAdapter);

	LARGE_INTEGER Li;
	::QueryPerformanceFrequency(&Li);
	gSystem->ClockFreq = Li.QuadPart;

	return true;
}

void System::Shutdown()
{
	gSystem->pDxgiAdapter->Release();
	gSystem->pDxgiFactory->Release();
	ZeroThat(gSystem);

#if defined(_DEBUG)
	IDXGIDebug *pDebug1;
	::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug1));
	pDebug1->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	pDebug1->Release();
#endif
}

s64 System::GetClock() 
{
	LARGE_INTEGER Li;
	::QueryPerformanceCounter(&Li);
	return Li.QuadPart;
}

f32 System::GetElapsedSeconds(s64 Start, s64 End) 
{
	return ((f32)(End - Start) / (f32)gSystem->ClockFreq);
}

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
bool FS::DoesFileExist(const TCHAR *Path)
{
	WIN32_FILE_ATTRIBUTE_DATA Fad;
	if (::GetFileAttributesEx(Path, GetFileExInfoStandard, &Fad))
		return true;
	return false;
}

size_t FS::GetFileSize(const TCHAR *Path)
{
	WIN32_FILE_ATTRIBUTE_DATA Fad;
	if (::GetFileAttributesEx(Path, GetFileExInfoStandard, &Fad)) {
		return ((size_t)Fad.nFileSizeHigh << 32 | Fad.nFileSizeLow);
	}
	return 0;
}

uint FS::GetNumFiles(const TCHAR *Path)
{
	uint num = 0;
	bool stop = false;

	WIN32_FIND_DATA Fd;
	HANDLE F = ::FindFirstFile(Path, &Fd);
	if (F == (HANDLE)-1) {
		return 0; // EXIT //
	}

	do {
		if (Fd.dwFileAttributes != INVALID_FILE_ATTRIBUTES && Fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			++num;
	} while (::FindNextFile(F, &Fd));

	return num;
}

void *FS::ReadEntireFile(const TCHAR *Path, size_t *pOutSize)
{
	void *pBuffer = nullptr;

	HANDLE F = ::CreateFile(Path, GENERIC_READ, 0x01, nullptr, OPEN_EXISTING, 128, nullptr);
	if (F != (HANDLE)-1) 
	{
		LARGE_INTEGER Li;
		::GetFileSizeEx(F, &Li);

		pBuffer = malloc(Li.QuadPart + 1);
		((char *)pBuffer)[Li.QuadPart] = 0;

		if (::ReadFile(F, pBuffer, Li.LowPart, nullptr, nullptr) && Li.HighPart) {
			::ReadFile(F, (void *)(((intptr_t)pBuffer) + Li.LowPart), Li.HighPart, nullptr, nullptr);
		}

		*pOutSize = Li.QuadPart;
		::CloseHandle(F);
	}
	else 
	{
	}

	return pBuffer;
}

bool FS::WriteEntireFile(const TCHAR *Path, void *pBuffer, size_t BufferSize)
{
	return false;
}

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
FS::CMappedFile::CMappedFile() : m_Name()
{
	m_Handle = (HANDLE)-1;
	m_pView = nullptr;
	m_Size = 0;
}

FS::CMappedFile::CMappedFile(const TCHAR *Path) : m_Name(Path)
{
	m_Handle = (void *)::CreateFile(Path, GENERIC_READ | GENERIC_WRITE, 0x01, nullptr, OPEN_EXISTING, 128, nullptr);
	if ((HANDLE)m_Handle != (HANDLE)-1)
	{
		LARGE_INTEGER Li;
		::GetFileSizeEx((HANDLE)m_Handle, &Li);
		m_Size = Li.QuadPart;

		HANDLE Mapping = ::CreateFileMappingA((HANDLE)m_Handle, nullptr, PAGE_READWRITE, 0, 0, nullptr);
		m_pView = ::MapViewOfFile(Mapping, FILE_MAP_WRITE, 0, 0, m_Size);
	}
	else 
	{
		m_pView = nullptr;
		m_Name.Free();
		m_Size = 0;
	}
}

FS::CMappedFile::~CMappedFile()
{
	Close();
}

void FS::CMappedFile::Close() 
{
	if ((HANDLE)m_Handle != (HANDLE)-1)
	{
		::UnmapViewOfFile(m_pView);
		HANDLE Mapping = ::CreateFileMapping((HANDLE)m_Handle, nullptr, PAGE_READWRITE, 0, 0, nullptr);
		::CloseHandle(Mapping);
		::CloseHandle((HANDLE)m_Handle);

		m_Handle = (void *)(HANDLE)-1;
		m_pView = nullptr;
		m_Name.Free();
		m_Size = 0;
	}
}