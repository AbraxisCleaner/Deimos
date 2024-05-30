#if !defined(_ENGINE_FILESYSTEM_H_)
#define _ENGINE_FILESYSTEM_H_

#include <pch.h>

namespace Engine
{
    bool ReadEntireFile(str_t Path, size_t *pOutSize);
    bool WriteEntireFile(str_t Path, void *pBuffer, size_t BufferSize);
}

#endif // _ENGINE_FILESYSTEM_H_