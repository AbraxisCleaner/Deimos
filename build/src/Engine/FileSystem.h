#if !defined(_ENGINE_FILESYSTEM_H_)
#define _ENGINE_FILESYSTEM_H_

#include <pch.h>
#include <Stl/String.h>

namespace Engine
{
    bool ReadEntireFile(str_t Path, size_t *pOutSize);
    bool WriteEntireFile(str_t Path, void *pBuffer, size_t BufferSize);

    enum EFileShare
    {
        eFileShareRead = 0x01,
        eFileShareWrite = 0x02,
        eFileShareDelete = 0x04,
        eFileShareReadWrite = 0x01|0x02,
        eFileShareAll = 0x01|0x02|0x04,
    };

    enum EFileOpen
    {
        eFileOpenNew = 1,
        eFileOpenExisting = 3,
        eFileOpenOverwrite = 2,
        eFileOpenTruncate = 5,
    };

    struct IFileHandle
    {
        IFileHandle();
        IFileHandle(str_t Path, EFileOpen OpenMode, EFileShare ShareMode);
        ~IFileHandle();

        void Close();
        bool IsDirty();

    private:
        void *m_Handle;
        TString<TCHAR> m_Name;
        EFileShare m_ShareMode;
        size_t m_Size;
        size_t m_LastWrite;
    };

    struct IMappedFile
    {
        IMappedFile();
        IMappedFile(str_t Path);
        ~IMappedFile();
        void Close();

    private:
        void *m_Handle;
        void *m_pView;
        TString<TCHAR> m_Name;
        size_t m_Size;
    };
}

#endif // _ENGINE_FILESYSTEM_H_