#ifndef _STL_CONTAINER_H_
#define _STL_CONTAINER_H_

/* -------------------------------------------------------------------------------------------------------------- */
template <typename T>
struct TArray
{
	TArray()
	{
		m_pElements = nullptr;
		m_uCount = 0;
		m_uCap = 0;
	}

	~TArray()
	{
		Release();
	}

	void Free()
	{
		if (m_pElements) {
			free(m_pElements);
			m_pElements = nullptr;
			m_uCount = 0;
			m_uCap = 0;
		}
	}

	// Ensures that there is space for ('m_uCount' + 'Count') elements.
	void Reserve(uint Count)
	{
		if (m_pElements) {
			auto t = m_uCount + Count;
			if (t > m_uCap) {
				auto oldCap = m_uCap;
				m_uCap = ALIGN8(t);
				m_pElements = (T *)realloc(m_pElements, m_uCap * sizeof(T));
				new(&m_pElements[oldCap]) T[m_uCap - oldCap];
			}
		}
		else {
			m_uCap = ALIGN8(Count);
			m_uCount = 0;
			m_pElements = (T *)malloc(m_uCap * sizeof(T));
			new(m_pElements) T[m_uCap];
		}
	}

	T *Append(uint uCount = 1, T *pSource = nullptr)
	{
		T *pDst = nullptr;

		if (!m_pElements) {
			m_uCount = uCount;
			m_uCap = ALIGN8(m_uCount);
			m_pElements = (T *)malloc(m_uCap * sizeof(T));
			new(m_pElements) T[m_uCap];
			pDst = m_pElements;
		}
		else {
			auto t = m_uCount + uCount;
			if (t > m_uCap) {
				auto oldCap = m_uCap;
				m_uCap = ALIGN8(t);
				m_pElements = (T *)realloc(m_pElements, m_uCap * sizeof(T));
				new(&m_pElements[oldCap]) T[m_uCap - oldCap];
			}
			pDst = &m_pElements[m_uCount];
		}

		if (pSource)
			memcpy(pDst, pSource, uCount * sizeof(T));
		m_uCount += uCount;
		return pDst;
	}

	T *begin() { return m_pElements; }
	T *end() { return &m_pElements[m_uCount]; }

	inline T *GetPtr() { return m_pElements; }
	inline uint GetCapacity() { return m_uCap; }
	inline uint Getcount() { return m_uCount; }

	inline T &operator [](uint it) { return m_pElements[it]; }
	inline operator T *() { return m_pElements; }

private:
	T *m_pElements;
	uint m_uCount;
	uint m_uCap;
};


#endif // _STL_CONTAINER_H_