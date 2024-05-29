#if !defined(_ARENA_H_)
#define _ARENA_H_

#include <pch.h>

/* ---------------------------------------------------------------------------------------------------- */
struct SMemoryArena
{
	uchar *pFirst;
	uchar *pLast;
	uchar *pNext;
	size_t Size;
	bool FailOnOverflow;
};
SMemoryArena AllocMemoryArena(size_t Size, bool FailOnOverflow);
void FreeMemoryArena(SMemoryArena *pArena);
void *ArenaAlloc(SMemoryArena *pArena, size_t Size);
void ZeroArena(SMemoryArena *pArena);

/* ---------------------------------------------------------------------------------------------------- */
struct CHeapAllocator
{
	CHeapAllocator() = default;
	CHeapAllocator(size_t Size);

	void Release();
	void *Alloc(size_t Size);
	void Free(void *Ptr);
	void *Realloc(void *OldPtr, size_t NewSize);

protected:
	struct SBlock
	{
		u32 Free;
		u32 Size;
		SBlock *pNext;
	};

	SBlock *m_pFirst;
	size_t m_Size;
};

#endif // _ARENA_H_