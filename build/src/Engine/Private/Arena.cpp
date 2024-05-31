#include <pch.h>
#include <Engine/Arena.h>

template <typename T> TArena<T>::TArena()
{
	m_pBaseAddr = nullptr;
	m_pNext = nullptr;
	m_Size = 0;
	m_IsRingBuffer = false;
}

template <typename T> TArena<T>::TArena(size_t Size, bool IsRingBuffer /* = true */)
{
	m_Size = ALIGN(Size, sizeof(T));
	m_pBaseAddr = (T *)::malloc(m_Size);
	memset(m_pBaseAddr, 0, m_Size);
	m_pNext = m_pBaseAddr;
	m_IsRingBuffer = IsRingBuffer;
}

template <typename T> void TArena<T>::Release()
{
	if (m_pBaseAddr) {
		free(m_pBaseAddr);
		m_pBaseAddr = nullptr;
		m_pNext = nullptr;
		m_Size = 0;
		m_IsRingBuffer = false;
	}
}

template <typename T> T *TArena<T>::Alloc(uint Count /* = 1 */)
{
	auto pDst = m_pNext;
	m_pNext += Count;
	if ((uintptr_t)m_pNext > (((uintptr_t)m_pBaseAddr) + m_Size)) {
		if (m_IsRingBuffer) {
			m_pNext = (m_pBaseAddr + Count);
			pDst = m_pBaseAddr;
		}
		else {
			pDst = nullptr;
		}
	}
	return pDst;
}

template <typename T> void TArena<T>::Undo(uint Count /* = 1 */) 
{
	m_pNext -= Count;
	if (m_pNext < m_pBaseAddr) {
		m_pNext = m_pBaseAddr;
	}
}

template <typename T> void TArena<T>::Reset()
{
	m_pNext = m_pBaseAddr;
	memset(m_pBaseAddr, 0, m_Size);
}

/* ------------------------------------------------------------------------------------------------------------------------ */
CStackArena::CStackArena()
{
	m_pBaseAddr = nullptr;
	m_pNext = nullptr;
	m_Size = 0;
	m_IsRingBuffer = false;
}

CStackArena::CStackArena(size_t Size, bool IsRingBuffer /* = true */)
{
	m_pBaseAddr = malloc(Size);
	memset(m_pBaseAddr, 0, Size);
	m_pNext = (uchar *)m_pBaseAddr;
	m_IsRingBuffer = IsRingBuffer;
}

void CStackArena::Release() 
{
	if (m_pBaseAddr) {
		free(m_pBaseAddr);
		m_pBaseAddr = nullptr;
		m_pNext = nullptr;
		m_Size = 0;
		m_IsRingBuffer = false;
	}
}

void *CStackArena::Alloc(size_t Size) 
{
	void *pDst = m_pNext;
	m_pNext += Size;
	if ((uintptr_t)m_pNext > (((uintptr_t)m_pBaseAddr) + m_Size)) {
		if (m_IsRingBuffer) {
			m_pNext = (uchar *)(((uintptr_t)m_pBaseAddr) + Size);
			pDst = m_pBaseAddr;
		}
		else {
			pDst = nullptr;
		}
	}
	return pDst;
}

void CStackArena::Undo(size_t Size)
{
	m_pNext -= Size;
	if ((uintptr_t)m_pNext < (uintptr_t)m_pBaseAddr) {
		m_pNext = (uchar *)m_pBaseAddr;
	}
}

void CStackArena::Reset()
{
	m_pNext = (uchar *)m_pBaseAddr;
	memset(m_pBaseAddr, 0, m_Size);
}