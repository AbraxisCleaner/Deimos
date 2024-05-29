#include <pch.h>
#include <Allocator.h>

/* ---------------------------------------------------------------------------------------------------- */
SMemoryArena AllocMemoryArena(size_t Size, bool FailOnOverflow)
{
	SMemoryArena Arena = {};
	Arena.pFirst = (uchar *)malloc(Size);
	memset(Arena.pFirst, 0, Size);
	Arena.Size = Size;
	Arena.pLast = Arena.pFirst + Size;
	Arena.pNext = Arena.pFirst;
	Arena.FailOnOverflow = FailOnOverflow;
	return Arena;
}

void FreeMemoryArena(SMemoryArena *pArena)
{
	free(pArena->pFirst);
	ZeroThat(pArena);
}

void *ArenaAlloc(SMemoryArena *pArena, size_t Size)
{
	auto pDst = pArena->pNext;
	pArena->pNext += Size;
	if (pArena->pNext > pArena->pLast) {
		if (pArena->FailOnOverflow)
			return NULL;
		pArena->pNext = (pArena->pFirst + Size);
		pDst = pArena->pFirst;
	}
	return pDst;
}

void ZeroArena(SMemoryArena *pArena)
{
	memset(pArena->pFirst, 0, pArena->Size);
	pArena->pNext = pArena->pFirst;
}

/* ---------------------------------------------------------------------------------------------------- */
CHeapAllocator::CHeapAllocator(size_t Size)
{
	m_pFirst = (SBlock *)malloc(Size);
	m_pFirst->Free = true;
	m_pFirst->pNext = (SBlock *)(((intptr_t)m_pFirst) + (Size - (2 * Size)));
	m_pFirst->Size = (Size - (2 * Size));
	m_Size = Size;
}

void CHeapAllocator::Release()
{
	free(m_pFirst);
	m_pFirst = NULL;
	m_Size = 0;
}

void *CHeapAllocator::Alloc(size_t Size)
{
	auto pMem = m_pFirst;
	while (pMem->pNext) {
		if (pMem->Free && pMem->Size >= Size) {
			pMem->Free = false;

			if (pMem->Free > (Size + sizeof(SBlock))) {
				auto pNext = (SBlock *)(((intptr_t)pMem) + (Size + sizeof(SBlock)));
				pNext->Free = true;
				pNext->pNext = pMem->pNext;
				pNext->Size = (pMem->Size - (Size + sizeof(SBlock)));
				pMem->pNext = pNext;
				pMem->Size = (Size - sizeof(SBlock));
			}

			return (void *)(((intptr_t)pMem) + sizeof(SBlock));
		}
		pMem = pMem->pNext;
	}
	return NULL;
}

void CHeapAllocator::Free(void *Ptr)
{
	auto pMem = (SBlock *)(((intptr_t)Ptr) - sizeof(SBlock));
	pMem->Free = true;

	while (pMem->pNext && pMem->pNext->Free) {
		pMem->Size += (pMem->pNext->Size + sizeof(SBlock));
		pMem->pNext = pMem->pNext->pNext;
	}
}

void *CHeapAllocator::Realloc(void *OldPtr, size_t NewSize)
{
	auto pMem = (SBlock *)(((intptr_t)OldPtr) - sizeof(SBlock));
	pMem->Free = true;

	auto OldSize = pMem->Size;

	while (pMem->pNext && pMem->pNext->Free) {
		pMem->Size += (pMem->pNext->Size + sizeof(SBlock));
		pMem->pNext = pMem->pNext->pNext;
	}

	if (pMem->Size >= NewSize) {
		pMem->Free = false;

		if (pMem->Size > (NewSize + sizeof(SBlock))) {
			auto pNext = (SBlock *)(((intptr_t)pMem) + (NewSize + sizeof(SBlock)));
			pNext->Free = true;
			pNext->pNext = pMem->pNext;
			pNext->Size = (pMem->Size - (NewSize + sizeof(SBlock)));
			pMem->pNext = pNext;
			pMem->Size = (NewSize - sizeof(SBlock));
		}

		return (void *)(((intptr_t)pMem) + sizeof(SBlock));
	}

	void *pDst = this->Alloc(NewSize);
	if (NewSize > OldSize)
		memcpy(pDst, OldPtr, OldSize);
	else
		memcpy(pDst, OldPtr, NewSize);
	return pDst;
}