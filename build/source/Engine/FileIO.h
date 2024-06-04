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

#endif // _ENGINE_FILESYSTEM_H_