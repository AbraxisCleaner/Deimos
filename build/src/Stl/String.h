#if !defined(_STL_STRING_H_)
#define _STL_STRING_H_

#include <pch.h>

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template <typename T> uint StrLen(const T *Str);
template <typename T> bool StrCmp(const T *Left, const T *Right);

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
class CStringHeader
{
public:
	CStringHeader()
	{
		m_pAddr = nullptr;
		m_Size = 0;
		m_NumReferences = 0;
	}

	void *Resize(size_t NewSize) 
	{
		if (m_pAddr) {
			auto OldSize = m_Size;
			m_Size = ALIGN16(NewSize);
			m_pAddr = realloc(m_pAddr, m_Size);
			memset((void *)(((intptr_t)m_pAddr) + OldSize), 0, (m_Size - OldSize));
		}
		else {
			m_Size = ALIGN16(NewSize);
			m_pAddr = malloc(m_Size);
			memset(m_pAddr, 0, m_Size);
		}
	}

	void IncrementReference() {
		++m_NumReferences;
	}

	void DecrementReference() {
		--m_NumReferences;
		if (!m_NumReferences) {
			delete this;
		}
	}

	inline size_t GetSize() const { return m_Size; }
	inline void *GetAddr() const { return m_pAddr; }

private:
	void *m_pAddr;
	size_t m_Size;
	uint m_NumReferences;
};

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template <typename T = char> struct TString;

/* @FIXME: Leaning very heavily on operators right now. */
template <> struct TString<char> 
{
	TString() 
	{
		m_pHeader = nullptr;
		m_Length = 0;
	}

	TString(const char *Ptr, uint Length) 
	{
		m_Length = Length;
		m_pHeader = new CStringHeader;
		m_pHeader->Resize(Length + 1);
		memcpy(m_pHeader->GetAddr(), Ptr, Length);
	}
	TString(const char *Ptr) : TString(Ptr, (uint)StrLen(Ptr)) {}

	~TString()
	{
		if (m_pHeader) {
			m_pHeader->DecrementReference();
		}
	}

	void Release()
	{
		m_pHeader->DecrementReference();
	}

	char *Set(const char *Ptr, uint Length)
	{
		m_Length = Length;
		if (m_Length > m_pHeader->GetSize()) {
			m_pHeader->Resize(m_Length);
		}
		memcpy(m_pHeader->GetAddr(), Ptr, m_Length);
	}
	inline char *Set(const char *Ptr) { return this->Set(Ptr, (uint)strlen(Ptr)); }

	char *Append(const char *Ptr, uint Length)
	{
		if (m_pHeader) {
			auto NewLength = (m_Length + Length);
			if (NewLength >= m_pHeader->Size) {
				auto OldSize = m_pHeader->Size;
				m_pHeader->Size = ALIGN16(NewLength + 1);
				m_pHeader->
			}
		}

		if (m_Ptr) {
			auto t = m_Length + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			auto pDst = &m_Ptr[m_Length];
			memcpy(pDst, Ptr, Length);
			m_Length = t;
			return pDst;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline char *Append(const char *Ptr) { return this->Append(Ptr, (uint)strlen(Ptr)); }

	char *Prepend(const char *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_Length + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			memcpy(&m_Ptr[Length], m_Ptr, m_Length);
			memcpy(m_Ptr, Ptr, Length);
			m_Length = t;
			return m_Ptr;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline char *Prepend(const char *Ptr) { return this->Prepend(Ptr, (uint)strlen(Ptr)); }

	char *Insert(const char *Ptr, uint Length, uint Where)
	{
		if (m_Ptr) {
			if (Where > m_Length)
				return this->Append(Ptr, Length);

			auto t = m_Length + Length;

			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}

			memcpy(&m_Ptr[Where + Length], &m_Ptr[Where], m_Length - Where);
			memcpy(&m_Ptr[Where], Ptr, Length);
			m_Length = t;
			return &m_Ptr[Where];
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline char *Insert(const char *Ptr, uint Where) { return this->Insert(Ptr, (uint)strlen(Ptr), Where); }

	inline char *Find(const char Ch) 
	{
		for (auto c = m_Ptr; *c; ++c)
			if (*c == Ch)
				return c;
		return NULL;
	}
	inline char *FindLast(const char Ch)
	{
		for (auto c = &*this[m_Length]; c != (char *)*this; --c)
			if (*c == Ch)
				return c;
		return NULL;
	}

	inline operator char *() { return (char *)(((intptr_t)m_pHeader) + sizeof(*m_pHeader)); }
	inline char &operator [](uint Index) { return ((char *)(((intptr_t)m_pHeader) + sizeof(*m_pHeader)))[Index]; }

private:
	CStringHeader *m_pHeader;
	uint m_Length; 
};

template <> struct TString<wchar_t>
{
	TString()
	{
		m_Ptr = L"";
		m_uCap = 0;
		m_Length = 0;
	}
	TString(const wchar_t *Ptr, uint Length)
	{
		m_uCap = ALIGN16(Length);
		m_Length = Length;
		m_Ptr = (wchar_t *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_Length);
	}
	TString(const wchar_t *Ptr)
	{
		m_Length = (uint)wcslen(Ptr);
		m_uCap = ALIGN16(m_Length);
		m_Ptr = (wchar_t *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_Length);
	}
	~TString()
	{
		if (m_Ptr != L"") {
			free(m_Ptr);
		}
	}

	void Free()
	{
		if (m_Ptr != L"") {
			free(m_Ptr);
			m_Ptr = L"";
			m_uCap = 0;
			m_Length = 0;
		}
	}

	wchar_t *Set(const wchar_t *Ptr, uint Length)
	{
		m_uCap = ALIGN16(Length);
		m_Length = Length;
		if (m_Ptr != L"") {
			m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
		}
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_Length);
	}
	inline wchar_t *Set(const wchar_t *Ptr) { return this->Set(Ptr, (uint)wcslen(Ptr)); }

	wchar_t *Append(const wchar_t *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_Length + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			auto pDst = &m_Ptr[m_Length];
			memcpy(pDst, Ptr, Length);
			m_Length = t;
			return pDst;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline wchar_t *Append(const wchar_t *Ptr) { return this->Append(Ptr, (uint)wcslen(Ptr)); }

	wchar_t *Prepend(const wchar_t *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_Length + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			memcpy(&m_Ptr[Length], m_Ptr, m_Length);
			memcpy(m_Ptr, Ptr, Length);
			m_Length = t;
			return m_Ptr;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline wchar_t *Prepend(const wchar_t *Ptr) { return this->Prepend(Ptr, (uint)wcslen(Ptr)); }

	wchar_t *Insert(const wchar_t *Ptr, uint Length, uint Where)
	{
		if (m_Ptr) {
			if (Where > m_Length)
				return this->Append(Ptr, Length);

			auto t = m_Length + Length;

			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}

			memcpy(&m_Ptr[Where + Length], &m_Ptr[Where], m_Length - Where);
			memcpy(&m_Ptr[Where], Ptr, Length);
			m_Length = t;
			return &m_Ptr[Where];
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline wchar_t *Insert(const wchar_t *Ptr, uint Where) { return this->Insert(Ptr, (uint)wcslen(Ptr), Where); }

	inline wchar_t *Find(const wchar_t Ch)
	{
		for (auto c = m_Ptr; *c; ++c)
			if (*c == Ch)
				return c;
		return NULL;
	}
	inline wchar_t *FindLast(const wchar_t Ch)
	{
		for (wchar_t *c = &m_Ptr[m_Length]; c != m_Ptr; --c)
			if (*c == Ch)
				return c;
		return NULL;
	}

	inline operator wchar_t *() { return m_Ptr; }
	inline wchar_t &operator [](uint Index) { return m_Ptr[Index]; }

private:
	wchar_t *m_Ptr;
	uint m_Length;
	uint m_uCap;
};

template <typename T> struct TStaticString;

template <> struct TStaticString<char> {
	TStaticString()
	{
		m_Ptr = "";
		m_uLength = 0;
	}
	TStaticString(const char *Ptr, uint Length) 
	{
		m_Ptr = Ptr;
		m_uLength = Length;
	}
	TStaticString(const char *Ptr) 
	{
		m_Ptr = Ptr;
		m_uLength = (uint)strlen(Ptr);
	}

	inline const char *Find(const char Ch)
	{
		for (auto c = m_Ptr; *c; ++c)
			if (*c == Ch)
				return c;
		return NULL;
	}
	inline const char *FindLast(const char Ch)
	{
		for (auto c = &m_Ptr[m_uLength]; c != m_Ptr; --c)
			if (*c == Ch)
				return c;
		return NULL;
	}

	inline const char &operator [](uint Index) { return m_Ptr[Index]; }
	inline operator const char *() { return m_Ptr; }
	inline operator const void *() { return m_Ptr; }

private:
	const char *m_Ptr;
	uint m_uLength;
};

#endif // _STL_STRING_H_