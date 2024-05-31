#if !defined(_ENGINE_ARENA_H_)
#define _ENGINE_ARENA_H_

#include <pch.h>

namespace Engine
{
	template <typename T>
	struct TArena
	{
		TArena();
		TArena(size_t Size, bool IsRingBuffer = true);
		void Release();
		T *Alloc(uint Count = 1);
		void Undo(uint Count = 1);
		void Reset();

		inline void operator =(TArena &&Arena) {
			Release();
			*this = std::move(Arena);
		}

	private:
		T *m_pBaseAddr;
		T *m_pNext;
		size_t m_Size;
		bool m_IsRingBuffer;
	};

	struct CStackArena
	{
		CStackArena();
		CStackArena(size_t Size, bool IsRingBuffer = true);
		void Release();
		void *Alloc(size_t Size);
		void Undo(size_t Size);
		void Reset();

	private:
		void *m_pBaseAddr;
		uchar *m_pNext;
		size_t m_Size;
		bool m_IsRingBuffer;
	};
}

#endif // _ENGINE_ARENA_H_
