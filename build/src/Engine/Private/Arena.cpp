#include <pch.h>
#include <Engine/Arena.h>

template <typename T> Engine::TArena<T>::TArena()
{
	m_pBaseAddr = nullptr;
	m_pNext = nullptr;
	m_Size = 0;
	m_IsRingBuffer = false;
}

template <typename T> Engine::TArena<T>::TArena(size_t Size, bool IsRingBuffer /* = true */)
{
	m_Size = ALIGN(Size, sizeof(T));
	m_pBaseAddr = (T *)::malloc(m_Size);
	memset(m_pBaseAddr, 0, m_Size);
	m_pNext = m_pBaseAddr;
	m_IsRingBuffer = IsRingBuffer;
}

template <typename T> void Engine::TArena<T>::Release()
{
	if (m_pBaseAddr) {
		free(m_pBaseAddr);
		m_pBaseAddr = nullptr;
		m_pNext = nullptr;
		m_Size = 0;
		m_IsRingBuffer = false;
	}
}

template <typename T> T *Engine::TArena<T>::Alloc(uint Count /* = 1 */)
{
	auto pDst = m_pNext;
	m_pNext += Count;
	if ((intptr_t)m_pNext > (((intptr_t)m_pBaseAddr) + m_Size)) {
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

template <typename T> void Engine::TArena<T>::Undo(uint Count /* = 1 */) 
{
	m_pNext -= Count;
	if (m_pNext < m_pBaseAddr) {
		m_pNext = m_pBaseAddr;
	}
}

template <typename T> void Engine::TArena<T>::Reset()
{
	m_pNext = m_pBaseAddr;
	memset(m_pBaseAddr, 0, m_Size);
}

/* ------------------------------------------------------------------------------------------------------------------------ */
Engine::CStackArena::CStackArena()
{
	m_pBaseAddr = nullptr;
	m_pNext = nullptr;
	m_Size = 0;
	m_IsRingBuffer = false;
}

Engine::CStackArena::CStackArena(size_t Size, bool IsRingBuffer /* = true */)
{
	m_pBaseAddr = malloc(Size);
	memset(m_pBaseAddr, 0, Size);
	m_pNext = (uchar *)m_pBaseAddr;
	m_IsRingBuffer = IsRingBuffer;
}

void Engine::CStackArena::Release() 
{
	if (m_pBaseAddr) {
		free(m_pBaseAddr);
		m_pBaseAddr = nullptr;
		m_pNext = nullptr;
		m_Size = 0;
		m_IsRingBuffer = false;
	}
}

void *Engine::CStackArena::Alloc(size_t Size) 
{
	void *pDst = m_pNext;
	m_pNext += Size;
	if ((intptr_t)m_pNext > ((intptr_t)m_pBaseAddr + m_Size)) {
		if (m_IsRingBuffer) {
			m_pNext = (uchar *)(((intptr_t)m_pBaseAddr) + Size);
			pDst = m_pBaseAddr;
		}
		else {
			pDst = nullptr;
		}
	}
	return pDst;
}

void Engine::CStackArena::Undo(size_t Size)
{
	m_pNext -= Size;
	if ((intptr_t)m_pNext < (intptr_t)m_pBaseAddr) {
		m_pNext = (uchar *)m_pBaseAddr;
	}
}

void Engine::CStackArena::Reset()
{
	m_pNext = (uchar *)m_pBaseAddr;
	memset(m_pBaseAddr, 0, m_Size);
}