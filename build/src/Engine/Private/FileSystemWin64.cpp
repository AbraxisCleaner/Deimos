#include <pch.h>
#include <Engine/FileSystem.h>
#include <Engine/Win64Layer.h>
#include <Engine/DebugLog.h>

#include <Windows.h>

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void *Engine::ReadEntireFile(str_t Path, size_t *pOutSize, void *(*pMallocFunc)(size_t Size))
{
    ::HANDLE F = ::CreateFile(Path, GENERIC_READ, 0x01, nullptr, OPEN_EXISTING, 128, nullptr);
    if (F != (HANDLE)-1) {
        ::LARGE_INTEGER Li;
        ::GetFileSizeEx(F, &Li);

        *pOutSize = Li.QuadPart;
        char *pBuffer = (char *)pMallocFunc(Li.QuadPart + 1);
        pBuffer[Li.QuadPart] = 0;

        if (::ReadFile(F, pBuffer, Li.LowPart, nullptr, nullptr) && Li.HighPart) {
            ::ReadFile(F, pBuffer + Li.LowPart, Li.HighPart, nullptr, nullptr);
        } else {
            LOGF("Unable to read from file: %s\n", Path);
        }

        ::CloseHandle(F);
        return pBuffer;
    }
    LOGF("Unable to open file for reading: %s\n", Path);
}

bool Engine::WriteEntireFile(str_t Path, void *pBuffer, size_t BufferSize)
{
    LOG("No implementation.\n");
    return false;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
Engine::IFileHandle::IFileHandle() : m_Name()
{
    m_Handle = (void *)(intptr_t)-1;
    m_Size = 0;
    m_LastWrite = 0;
    m_ShareMode = (EFileShare)0;
}

Engine::IFileHandle::IFileHandle(const TCHAR *Path, EFileOpen OpenMode, EFileShare ShareMode)
{
    m_Handle = (void *)::CreateFile(Path, GENERIC_READ | GENERIC_WRITE, ShareMode, NULL, OPEN_EXISTING, 128, NULL);
    if ((intptr_t)m_Handle != (intptr_t)-1) {
        LARGE_INTEGER Li;
        ::GetFileSizeEx(m_Handle, &Li);
        m_Size = Li.QuadPart;

        FILETIME Ft;
        ::GetFileTime(m_Handle, NULL, NULL, &Ft);
        m_LastWrite = (size_t)Ft.dwHighDateTime << 32 | Ft.dwLowDateTime;

        m_ShareMode = ShareMode;
        m_Name.Set(Path);
        LOGF("Opened file handle: %s\n", Path);
    }
    else {
        LOGF("Unable to open file for reading: %s\n", Path);
        m_Size = 0;
        m_LastWrite = 0;
        m_ShareMode = (EFileShare)0;
    }
}

Engine::IFileHandle::~IFileHandle() 
{
    if ((intptr_t)m_Handle != (intptr_t)-1) {
        Close();
    }
}

void Engine::IFileHandle::Close()
{
	::CloseHandle(m_Handle);
	m_Handle = (void *)(intptr_t)-1;
    m_Name.Free();
	m_Size = 0;
	m_LastWrite = 0;
	m_ShareMode = (EFileShare)0;
}

bool Engine::IFileHandle::IsDirty()
{
    WIN32_FILE_ATTRIBUTE_DATA Fad;
    ::GetFileInformationByHandleEx(m_Handle, FileStandardInfo, &Fad, sizeof(Fad));
    return (((size_t)Fad.ftLastWriteTime.dwHighDateTime << 32 | Fad.ftLastWriteTime.dwLowDateTime) > m_LastWrite) ? true : false;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
Engine::IMappedFile::IMappedFile() : m_Name()
{
    m_Handle = (void *)(intptr_t)-1;
    m_pView = NULL;
    m_Size = 0;
}

Engine::IMappedFile::IMappedFile(const TCHAR *Path)
{
    m_Handle = (void *)::CreateFile(Path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 128, NULL);
    if ((intptr_t)m_Handle != (intptr_t)-1) {
        LARGE_INTEGER Li;
        ::GetFileSizeEx(m_Handle, &Li);
        m_Size = Li.QuadPart;

        HANDLE Mapping = ::CreateFileMapping(m_Handle, NULL, PAGE_READWRITE, 0, 0, NULL);
        m_pView = ::MapViewOfFile(Mapping, FILE_MAP_WRITE, 0, 0, m_Size);
        
        m_Name.Set(Path);
        LOGF("Mapped file: %s\n", Path);
    }
    else {
        LOGF("Unable to map file: %s\n", Path);
    }
}

Engine::IMappedFile::~IMappedFile()
{
    if ((intptr_t)m_Handle != (intptr_t)-1) {
        this->Close();
    }
}

void Engine::IMappedFile::Close()
{
	HANDLE Mapping = ::CreateFileMapping(m_Handle, NULL, PAGE_READWRITE, 0, 0, NULL);
	::UnmapViewOfFile(m_pView);
	::CloseHandle(Mapping);
    ::CloseHandle(m_Handle);
    m_Handle = (void *)(intptr_t)-1;
    m_Name.Free();
    m_pView = NULL;
    m_Size = 0;
}

void Engine::IMappedFile::Append(char *pBuffer, size_t BufferSize)
{
    HANDLE Mapping = ::CreateFileMapping(m_Handle, NULL, PAGE_READWRITE, 0, 0, NULL);
    ::UnmapViewOfFile(m_pView);
    ::CloseHandle(Mapping);

    ::SetFilePointer(m_Handle, 0, 0, FILE_END);
    LARGE_INTEGER Li;
    Li.QuadPart = BufferSize;
    if (WriteFile(m_Handle, pBuffer, Li.LowPart, NULL, NULL) && Li.HighPart)
        WriteFile(m_Handle, pBuffer + Li.LowPart, Li.HighPart, NULL, NULL);

	Mapping = ::CreateFileMappingA(m_Handle, NULL, PAGE_READWRITE, 0, 0, NULL);
	m_pView = ::MapViewOfFile(Mapping, FILE_MAP_WRITE, 0, 0, m_Size);

    ::GetFileSizeEx(m_Handle, &Li);
    m_Size = Li.QuadPart;
}