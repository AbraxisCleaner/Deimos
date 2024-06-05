#if !defined(_STL_STRING_H_)
#define _STL_STRING_H_

#include <pch.h>

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* Specializations present in Stl/Private/String.cpp */

template <typename T> uint TStrlen(const T *Str);
template <typename T> bool TStrcmp(const T *Left, const T *Right);
template <typename T> const T *TEmptyString();

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template <typename T> 
class TString 
{
public:
	TString()
	{
		m_Ptr = (T *)TEmptyString<T>();
		m_uCap = 0;
		m_uLength = 0;
	}

	TString(const T *Ptr, uint Length)
	{
		m_uLength = Length;
		m_uCap = ALIGN16(m_uLength + 1);
		m_Ptr = (T *)malloc(m_uCap * sizeof(T));
		memset(m_Ptr, 0, m_uCap * sizeof(T));
		memcpy(m_Ptr, Ptr, m_uLength * sizeof(T));
	}
	TString(const T *Ptr) : TString(Ptr, TStrlen(Ptr)) {}

	TString(const TString &Other) {
		m_uLength = Other.m_uLength;
		m_uCap = ALIGN16(m_uLength + 1);
		m_Ptr = (T *)malloc(m_uCap * sizeof(T));
		memset(m_Ptr, 0, m_uCap * sizeof(T));
		memcpy(m_Ptr, Other.m_Ptr, m_uLength * sizeof(T));
	}

	~TString()
	{
		Free();
	}

	void Free()
	{
		if (m_Ptr != (T *)TEmptyString<T>()) {
			free(m_Ptr);
			m_Ptr = (T *)TEmptyString<T>();
			m_uCap = 0;
			m_uLength = 0;
		}
	}

	void Set(const T *Ptr, uint Length)
	{
		m_uLength = Length;
		m_uCap = ALIGN16(Length + 1);
		if (m_Ptr != (T *)TEmptyString<T>())
			m_Ptr = (T *)realloc(m_Ptr, m_uCap * Sizeof(T));
		else
			m_Ptr = (T *)malloc(m_uCap * sizeof(T));
		memset(m_Ptr, 0, m_uCap * sizeof(T));
		memcpy(m_Ptr, Ptr, Length * sizeof(T));
	}
	inline void Set(const T *Ptr) { Set(Ptr, TStrlen(Ptr)); }

	void Reserve(uint Addition) 
	{
		if (m_Ptr != (T *)TEmptyString<T>()) {
			auto t = m_uLength + Addition + 1;
			if (t >= m_uCap) {
				auto old = m_uCap;
				m_uCap = ALIGN16(t);
				m_Ptr = (T *)realloc(m_Ptr, m_uCap * sizeof(T));
				memset(&m_Ptr[old], 0, m_uCap - old);
			}
		}
		else {
			m_uCap = ALIGN16(Addition + 1);
			m_Ptr = (T *)malloc(m_uCap * sizeof(T));
			memset(m_Ptr, 0, m_uCap * sizeof(T));
		}
	}

	void Resize(uint NewSize)
	{
		if (m_Ptr != (T *)TEmptyString<T>()) {
			auto old = m_uCap;
			m_uCap = ALIGN16(NewSize + 1);
			m_Ptr = (T *)realloc(m_Ptr, m_uCap * sizeof(T));
			if (NewSize > old) {
				memset(&m_Ptr[old], 0, (m_uCap - old) * sizeof(T));
			}
			else {
				m_Ptr[m_uCap] = 0;
			}
		}
	}

	void Append(const T *Ptr, uint Length)
	{
		if (m_Ptr != (T *)TEmptyString<T>()) {
			auto t = (m_uLength + Length + 1);
			if (t >= m_uCap) {
				auto old = m_uCap;
				m_uCap = ALIGN16(t);
				m_Ptr = (T *)realloc(m_Ptr, m_uCap * sizeof(T));
				memset(&m_Ptr[old], 0, (m_uCap - old) * sizeof(T));
			}
			memcpy(&m_Ptr[m_uLength], Ptr, Length * sizeof(T));
			m_uLength = t;
		}
		else {
			m_uLength = Length;
			m_uCap = ALIGN16(Length + 1);
			m_Ptr = (T *)malloc(m_uCap * sizeof(T));
			memset(m_Ptr, 0, m_uCap * sizeof(T));
			memcpy(m_Ptr, Ptr, Length * sizeof(T));
		}
	}
	inline void Append(const T *Ptr) { Append(Ptr, TStrlen(Ptr)); }

	void Prepend(const T *Ptr, uint Length)
	{
		if (m_Ptr != (T *)TEmptyString<T>()) {
			auto t = (m_uLength + Length + 1);
			if (t >= m_uCap) {
				auto old = m_uCap;
				m_uCap = ALIGN16(t);
				m_Ptr = (T *)realloc(m_Ptr, m_uCap * sizeof(T));
				memset(&m_Ptr[old], 0, (m_uCap - old) * sizeof(T));
			}
			memcpy(&m_Ptr[Length], m_Ptr, m_uLength * sizeof(T));
			memcpy(m_Ptr, Ptr, Length * sizeof(T));
			m_uLength = t;
		}
		else {
			m_uLength = Length;
			m_uCap = ALIGN16(Length + 1);
			m_Ptr = (T *)malloc(m_uCap * sizeof(T));
			memset(m_Ptr, 0, m_uCap * sizeof(T));
			memcpy(m_Ptr, Ptr, Length * sizeof(T));
		}
	}
	inline void Prepend(const T *Ptr) { Prepend(Ptr, TStrlen(Ptr)); }

	void Insert(const T *Ptr, uint Length, uint Where)
	{
		if (m_Ptr != (T *)TEmptyString<T>()) {
			auto t = (m_uLength + Length + 1);
			if (t >= m_uCap) {
				auto old = m_uCap;
				m_uCap = ALIGN16(t);
				m_Ptr = (T *)realloc(m_Ptr, m_uCap * sizeof(T));
				memset(&m_Ptr[old], 0, (m_uCap - old) * sizeof(T));
			}
			memcpy(&m_Ptr[Where + Length], &m_Ptr[Where], (m_uLength - Where) * sizeof(T));
			memcpy(&m_Ptr[Where], Ptr, Length * sizeof(T));
			m_uLength = t;
		}
		else {
			m_uLength = Length;
			m_uCap = ALIGN16(Length + 1);
			m_Ptr = (T *)malloc(m_uCap * sizeof(T));
			memset(m_Ptr, 0, m_uCap * sizeof(T));
			memcpy(m_Ptr, Ptr, Length * sizeof(T));
		}
	}
	inline void Insert(const T *Ptr, uint Where) { Insert(Ptr, TStrlen(Ptr), Where); }
	inline void Insert(const T *Ptr, T *Where) { Insert(Ptr, TStrlen(Ptr), (uint)(Where - m_Ptr)); }

	inline T *Find(const T Ch) {
		for (auto i = 0; i != m_uLength; ++i) {
			if (m_Ptr[i] == Ch)
				return &m_Ptr[i];
		}
		return nullptr;
	}
	inline T *FindLast(const T Ch) {
		for (auto i = m_uLength; i >= 0; --i) {
			if (m_Ptr[i] == Ch)
				return &m_Ptr[i];
		}
	}

	// Returns (uint)-1 on failure.
	inline uint GetIndexOf(const T Ch) {
		for (auto i = 0; i != m_uLength; ++i) {
			if (m_Ptr[i] == Ch)
				return i;
		}
		return (uint)-1;
	}
	// Returns (uint)-1 on failure.
	inline uint GetLastIndexOf(const T Ch) {
		for (auto i = m_uLength; i >= 0; --i) {
			if (m_Ptr[i] == Ch)
				return i;
		}
		return (uint)-1;
	}

	inline void *Raw() { return m_Ptr; }
	inline uint Length() { return m_uLength; }
	inline uint Capacity() { return m_uCap; }

	// -----------------------------------
	// FilePath/Name stuff.
	// -----------------------------------
	inline T *GetFileExtension() { 
		return FindLast('.'); 
	}
	
	inline T *GetFileName() { 
		auto ptr = FindLast('\\'); 
		if (!ptr)
			ptr = FindLast('/');
		return (ptr) ? (ptr + 1) : m_Ptr; 
	}

	inline TString GetFilePath() {
		TString dst = *this;
		auto ptr = dst.GetFileName();
		*ptr = 0;
		return dst;
	}

	// -----------------------------------
	// Operators.
	// -----------------------------------
	inline operator T *() { return m_Ptr; }
	inline operator const T *() { return (const T *)m_Ptr; }
	inline T &operator [](uint Index) { return m_Ptr[Index]; }
	
	inline bool operator ==(const T *Other) { return TStrcmp(m_Ptr, Other); }
	inline bool operator ==(const TString &Other) { return TStrcmp(m_Ptr, Other.m_Ptr); }
	inline void operator +=(const T *Str) { Append(Str); }

protected:
	T *m_Ptr;
	uint m_uCap;
	uint m_uLength;
};

#endif // _STL_STRING_H_