#if !defined(_ENGINE_FILESYSTEM_H_)
#define _ENGINE_FILESYSTEM_H_

#include <pch.h>
#include <Stl/String.h>

namespace FS
{
    bool DoesFileExist(const TCHAR *Path);
    size_t GetFileSize(const TCHAR *Path);
    uint GetNumFiles(const TCHAR *DirPath);
    void *ReadEntireFile(const TCHAR *Path, size_t *pOutSize);
    bool WriteEntireFile(const TCHAR *Path, void *pBuffer, size_t BufferSize);

    enum EFileShare
    {
        eFileShareRead = 0x01,
        eFileShareWrite = 0x02,
        eFileShareDelete = 0x04,
        eFileShareReadWrite = 0x01|0x02,
        eFileShareAll = 0x01|0x02|0x04,
    };

    // Just mirror Win32.
    enum EFileOpen
    {
        eFileOpenNew = 1,
        eFileOpenExisting = 3,
        eFileOpenOverwrite = 2,
        eFileOpenTruncate = 5,
    };

    struct CMappedFile
    {
        CMappedFile();
        CMappedFile(const TCHAR *Path);
        ~CMappedFile();

        void Close();

        inline operator void *() { return m_pView; }

    private:
        void *m_Handle;
        void *m_pView;
        TString<TCHAR> m_Name;
        size_t m_Size;
    };
}

#endif // _ENGINE_FILESYSTEM_H_