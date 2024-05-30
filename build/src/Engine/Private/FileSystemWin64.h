#include <pch.h>
#include <Engine/Public/FileSystem.h>
#include <Engine/Public/DebugLog.h>

#include <Windows.h>

bool Engine::ReadEntireFile(str_t Path, size_t *pOutSize)
{
    ::HANDLE F = ::CreateFile(Path, GENERIC_READ, 0x01, nullptr, OPEN_EXISTING, 128, nullptr);
    if (F != (HANDLE)-1) {
        ::LARGE_INTEGER Li;
        ::GetFileSizeEx(F, &Li);

        *pOutSize = Li.QuadPart;
        char *pBuffer = (char *)malloc(Li.QuadPart + 1);
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