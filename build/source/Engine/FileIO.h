#ifndef _ENGINE_FILESYSTEM_H_
#define _ENGINE_FILESYSTEM_H_

struct SFileBlob
{
	size_t Size;
	void *pBuffer;

	inline void Release() {
		if (pBuffer)
			free(pBuffer);
		ZeroThis();
	}
};
bool ReadEntireFile(const TCHAR *Path, SFileBlob *pBlob);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct SMappedFile
{
	void *Handle;
	size_t Size;
	void *pView;

	void Release();
};
bool MapFile(const TCHAR *Path, SMappedFile *pOut);
void UnmapFile(SMappedFile *pFile);

#endif // _ENGINE_FILESYSTEM_H_