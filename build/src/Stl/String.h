#if !defined(_STL_STRING_H_)
#define _STL_STRING_H_

#include <pch.h>

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template <typename T> uint StrLen(const T *Str);
template <typename T> bool StrCmp(const T *Left, const T *Right);

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template <typename T = char> struct TString;

template <> struct TString<char> 
{
	TString() 
	{
		m_Ptr = "";
		m_uCap = 0;
		m_uLength = 0;
	}
	TString(const char *Ptr, uint Length) 
	{
		m_uCap = ALIGN16(Length);
		m_uLength = Length;
		m_Ptr = (char *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}
	TString(const char *Ptr) 
	{
		m_uLength = (uint)strlen(Ptr);
		m_uCap = ALIGN16(m_uLength);
		m_Ptr = (char *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}
	~TString()
	{
		if (m_Ptr != "") {
			free(m_Ptr);
		}
	}

	void Free()
	{
		if (m_Ptr != "") {
			free(m_Ptr);
			m_Ptr = "";
			m_uCap = 0;
			m_uLength = 0;
		}
	}

	char *Set(const char *Ptr, uint Length)
	{
		m_uCap = ALIGN16(Length);
		m_uLength = Length;
		if (m_Ptr != "") {
			m_Ptr = (char *)realloc(m_Ptr, m_uCap);
		}
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}
	inline char *Set(const char *Ptr) { return this->Set(Ptr, (uint)strlen(Ptr)); }

	char *Append(const char *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_uLength + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			auto pDst = &m_Ptr[m_uLength];
			memcpy(pDst, Ptr, Length);
			m_uLength = t;
			return pDst;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline char *Append(const char *Ptr) { return this->Append(Ptr, (uint)strlen(Ptr)); }

	char *Prepend(const char *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_uLength + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			memcpy(&m_Ptr[Length], m_Ptr, m_uLength);
			memcpy(m_Ptr, Ptr, Length);
			m_uLength = t;
			return m_Ptr;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline char *Prepend(const char *Ptr) { return this->Prepend(Ptr, (uint)strlen(Ptr)); }

	char *Insert(const char *Ptr, uint Length, uint Where)
	{
		if (m_Ptr) {
			if (Where > m_uLength)
				return this->Append(Ptr, Length);

			auto t = m_uLength + Length;

			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}

			memcpy(&m_Ptr[Where + Length], &m_Ptr[Where], m_uLength - Where);
			memcpy(&m_Ptr[Where], Ptr, Length);
			m_uLength = t;
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
		for (auto c = &m_Ptr[m_uLength]; c != m_Ptr; --c)
			if (*c == Ch)
				return c;
		return NULL;
	}

	inline operator char *() { return m_Ptr; }
	inline char &operator [](uint Index) { return m_Ptr[Index]; }

private:
	char *m_Ptr;
	uint m_uLength; 
	uint m_uCap;
};

template <> struct TString<wchar_t>
{
	TString()
	{
		m_Ptr = L"";
		m_uCap = 0;
		m_uLength = 0;
	}
	TString(const wchar_t *Ptr, uint Length)
	{
		m_uCap = ALIGN16(Length);
		m_uLength = Length;
		m_Ptr = (wchar_t *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}
	TString(const wchar_t *Ptr)
	{
		m_uLength = (uint)wcslen(Ptr);
		m_uCap = ALIGN16(m_uLength);
		m_Ptr = (wchar_t *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
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
			m_uLength = 0;
		}
	}

	wchar_t *Set(const wchar_t *Ptr, uint Length)
	{
		m_uCap = ALIGN16(Length);
		m_uLength = Length;
		if (m_Ptr != L"") {
			m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
		}
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}
	inline wchar_t *Set(const wchar_t *Ptr) { return this->Set(Ptr, (uint)wcslen(Ptr)); }

	wchar_t *Append(const wchar_t *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_uLength + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			auto pDst = &m_Ptr[m_uLength];
			memcpy(pDst, Ptr, Length);
			m_uLength = t;
			return pDst;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline wchar_t *Append(const wchar_t *Ptr) { return this->Append(Ptr, (uint)wcslen(Ptr)); }

	wchar_t *Prepend(const wchar_t *Ptr, uint Length)
	{
		if (m_Ptr) {
			auto t = m_uLength + Length;
			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}
			memcpy(&m_Ptr[Length], m_Ptr, m_uLength);
			memcpy(m_Ptr, Ptr, Length);
			m_uLength = t;
			return m_Ptr;
		}
		// else
		return this->Set(Ptr, Length);
	}
	inline wchar_t *Prepend(const wchar_t *Ptr) { return this->Prepend(Ptr, (uint)wcslen(Ptr)); }

	wchar_t *Insert(const wchar_t *Ptr, uint Length, uint Where)
	{
		if (m_Ptr) {
			if (Where > m_uLength)
				return this->Append(Ptr, Length);

			auto t = m_uLength + Length;

			if (t >= m_uCap) {
				auto OldCap = m_uCap;
				m_uCap = ALIGN16(t + 1);
				m_Ptr = (wchar_t *)realloc(m_Ptr, m_uCap * sizeof(wchar_t));
				memset(&m_Ptr[OldCap], 0, m_uCap - OldCap);
			}

			memcpy(&m_Ptr[Where + Length], &m_Ptr[Where], m_uLength - Where);
			memcpy(&m_Ptr[Where], Ptr, Length);
			m_uLength = t;
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
		for (wchar_t *c = &m_Ptr[m_uLength]; c != m_Ptr; --c)
			if (*c == Ch)
				return c;
		return NULL;
	}

	inline operator wchar_t *() { return m_Ptr; }
	inline wchar_t &operator [](uint Index) { return m_Ptr[Index]; }

private:
	wchar_t *m_Ptr;
	uint m_uLength;
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