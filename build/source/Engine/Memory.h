#if !defined(_ENGINE_MEMORY_ARENA_H_)
#define _ENGINE_MEMORY_ARENA_H_

#include <pch.h>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct dmRingArena_t 
{
	dmRingArena_t() = default;
	dmRingArena_t(const size_t inSize);

	bool Initialize(const size_t inSize);
	void Release();
	void Reset();
	void *Push(const size_t inSize);
	void Pop(const size_t inSize);

	/* ----- Members ----- */
	uintptr_t m_floor;
	uintptr_t m_ceiling;
	uintptr_t m_next;
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct dmHeap_t
{
	dmHeap_t() = default;
	dmHeap_t(const size_t inSize) { this->Initialize(inSize); }

	bool  Initialize(const size_t inSize);
	void  Release();
	void  Reset();
	void *Alloc(const size_t inSize);
	void *Realloc(const void *inPtr, const size_t inSize);
	void  Free(const void *inPtr);
	uint  DefragAll();

	/* ----- Members ----- */
	struct header_t {
		header_t *pNext;
		size_t size;
	};
	uint  DefragOne(header_t *pHeader);

	void *m_baseAddr;
	header_t *m_pFirst;
	size_t m_allocatedSize;
};

#endif // _ENGINE_MEMORY_ARENA_H_