#ifndef _STL_CONTAINER_H_
#define _STBL_CONTAINER_H_

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

/* -------------------------------------------------------------------------------------------------------------- */
static inline uint _fnva1(uchar *Ptr, size_t Size)
{
	uint h = 0x811c9dc5; 
	for (auto i = 0; i != Size; ++i) { 
		h *= 0x01000193; 
		h ^= Ptr[i]; 
	}
	return h;
}

template <typename T, uint _BucketSize = 4, uint(*_HashFunc)(uchar *Ptr, size_t Size) = _fnva1>
struct TMap
{
	TMap()
	{
		m_lpBuckets = nullptr;
	}

	~TMap()
	{
		if (m_lpBuckets) {
			for (auto i = 0; i != 32; ++i) {
				if (m_lpBuckets[i])
					delete m_lpBuckets[i];
			}
			free(m_lpBuckets);
		}
	}

	T &Get(const char *Ptr)
	{
		uint Length = (uint)strlen(Ptr);
		uint h = _HashFunc(Ptr, Length) % 32;

		if (!m_lpBuckets) {
			m_lpBuckets = (SBucket **)malloc(32 * sizeof(intptr_t));
			memset(m_lpBuckets, 0, 32 * sizeof(intptr_t));
		}

		if (!m_lpBuckets[h]) {
			m_lpBuckets[h] = new SBucket;
		}
	}

	inline T &operator [](const char *Ptr) { return Get(Ptr); }

private:
	T &GetFromBucket(SBucket &pBucket, const char *Ptr, uint Length)
	{
		for (auto i = 0; i != _BucketSize; ++i) {
			if (!pBucket->Keys[i]) {
				pBucket->Keys[i] = (char *)malloc(Length + 1);
				memcpy(pBucket->Keys[i], Ptr, Length);
				pBucket->Keys[i][Length] = 0;
				return pBucket->Elements[i];
			}
			if (!pNext) {
				pNext = new SBucket;
			}
			return GetFromBucket(pNext);
		}
	}

	struct SBucket 
	{
		SBucket() { memset(Keys, 0, _BucketSize * sizeof(intptr_t)); pNext = NULL; }
		~SBucket() {
			for (auto i = 0; i != _BucketSize; ++i) {
				if (Keys[i])
					free(Keys[i]);
			}
			if (pNext)
				delete pNext;
		}

		T Elements[_BucketSize];
		char *Keys[_BucketSize];
		SBucket *pNext;
	};

	SBucket **m_lpBuckets;
};

#endif // _STL_CONTAINER_H_
