#include <pch.h>
#include <Engine/Memory.h>

dmRingArena_t::dmRingArena_t(const size_t inSize) 
{
	this->Initialize(inSize);
}

bool dmRingArena_t::Initialize(const size_t inSize) 
{
	m_floor = (uintptr_t)malloc(inSize);
	if (m_floor) {
		memset((void *)m_floor, 0, inSize);
		m_ceiling = (m_floor + inSize);
		m_next = m_floor;
	}
	return (m_floor != 0);
}

void dmRingArena_t::Release() 
{
	if (m_floor) {
		free((void *)m_floor);
		ZeroThat(this);
	}
}

void dmRingArena_t::Reset()
{
	m_next = m_floor;
	memset((void *)m_floor, 0, (size_t)(m_ceiling - m_floor));
}

void *dmRingArena_t::Push(const size_t inSize) 
{
	auto dst = m_next;
	m_next += inSize;
	if (m_next > m_ceiling) {
		m_next = (m_floor + inSize);
		dst = m_floor;
	}
	return (void *)dst;
}

void dmRingArena_t::Pop(const size_t inSize)
{
	m_next -= inSize;
	if (m_next < m_floor) {
		m_next = m_floor;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool dmHeap_t::Initialize(size_t inSize) 
{
	m_baseAddr = malloc(inSize);
	if (m_baseAddr) {
		memset(m_baseAddr, 0, inSize);
		m_pFirst = (header_t *)m_baseAddr;
		m_pFirst->pNext = nullptr;
		m_pFirst->size = inSize - sizeof(*m_pFirst);
	}
	return (m_baseAddr != nullptr);
}

void dmHeap_t::Release()
{
	free(m_baseAddr);
	m_baseAddr = nullptr;
	m_pFirst = nullptr;
	m_allocatedSize = 0;
}

void dmHeap_t::Reset()
{
	memset(m_baseAddr, 0, m_allocatedSize);
	m_pFirst = (header_t *)m_baseAddr;
	m_pFirst->size = m_allocatedSize;
}

void *dmHeap_t::Alloc(const size_t inSize)
{
	header_t *pPrev = nullptr, *pThis = m_pFirst;

	while (pThis) {

		// If 'pThis' is big enough for inSize + a tracking size_t...
		if (pThis->size >= (inSize + sizeof(size_t))) {
			pPrev->pNext = pThis->pNext; // Unlink 'pThis'.

			// If 'pThis' can be bisected...
			if (pThis->size > (inSize + sizeof(size_t) + sizeof(header_t))) {
				// [h . . . . . . . . . .]  -> [h . . . h . . . . . .]
				auto pInsert = (header_t *)(((uintptr_t)pThis) + inSize + sizeof(size_t));
				pInsert->pNext = pPrev->pNext;
				pInsert->size = (pThis->size - (inSize));
				pPrev->pNext = pInsert;
			}

			*(size_t *)pThis = inSize; // Insert tracking size_t.
			return (void *)(((uintptr_t)pThis) + sizeof(size_t)); // Return usable addr.
		}

		pPrev = pThis;
		pThis = pThis->pNext;
	}

	return nullptr;
}

void *dmHeap_t::Realloc(const void *inPtr, size_t inSize)
{
	auto pHeader = (header_t *)(((uintptr_t)inPtr) - sizeof(size_t));
	size_t oldSize = *(size_t *)pHeader;
	
	auto pDst = this->Alloc(inSize);
	if (oldSize >= inSize)
		memcpy(pDst, inPtr, inSize);
	else
		memcpy(pDst, inPtr, oldSize);

	pHeader->size = oldSize;
	pHeader->pNext = m_pFirst;
	m_pFirst = pHeader;

	//DefragOne(pHeader);

	return pDst;
}

void dmHeap_t::Free(const void *inPtr)
{
	auto pHeader = (header_t *)(((uintptr_t)inPtr) - sizeof(size_t));
	auto size = *(size_t *)pHeader;
	pHeader->pNext = m_pFirst;
	pHeader->size = size;
}

uint dmHeap_t::DefragAll()
{
	uint numReclaimed = 0; // The number of blocks that have been reclaimed.

	for (auto pHeader = m_pFirst; 
		 pHeader != nullptr; 
		 pHeader = pHeader->pNext) 
	{
		// While we keep finding contiguous blocks at the end of 'pHeader', keep running the defrag loop.
		while (true) {
			// temp
			header_t *pPrev = nullptr, *pNext = m_pFirst;

			while (pNext != nullptr) {
				// if 'pNext' is directly contiguous with 'pHeader'...
				if ((uintptr_t)pNext == (((uintptr_t)pHeader) + pHeader->size)) {
					break;
				}
				pPrev = pNext;
				pNext = pNext->pNext;
			}

			if (pNext) {
				++numReclaimed;
				pHeader->size += pNext->size; // Let 'pHeader' consume 'pNext'.
				pPrev->pNext = pNext->pNext; // Unlink 'pNext'.
			} else {
				break; // 'pHeader' has not consumed anything, so break the loop.
			}
		}
	}

	return numReclaimed;
}

uint dmHeap_t::DefragOne(header_t *pHeader)
{
	uint numReclaimed = 0;

	while (true) {
		// temp
		header_t *pPrev = nullptr, *pNext = m_pFirst;

		while (pNext != nullptr) {
			// if 'pNext' is directly contiguous with 'pHeader'...
			if ((uintptr_t)pNext == (((uintptr_t)pHeader) + pHeader->size)) {
				break;
			}
			pPrev = pNext;
			pNext = pNext->pNext;
		}

		if (pNext) {
			++numReclaimed;
			pHeader->size += pNext->size; // Let 'pHeader' consume 'pNext'.
			pPrev->pNext = pNext->pNext; // Unlink 'pNext'.
		} else {
			break; // 'pHeader' has not consumed anything, so break the loop.
		}
	}

	return numReclaimed;
}