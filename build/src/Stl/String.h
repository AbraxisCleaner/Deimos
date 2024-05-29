#if !defined(_STL_STRING_H_)
#define _STL_STRING_H_

#include <pch.h>

/* -------------------------------------------------------------------------------------------------------------- */
template <uint _Cap>
struct TSizedString
{
	TSizedString()
	{
		memset(m_Ptr, 0, _Cap);
		m_uLength = 0;
	}
	
	TSizedString(const char *Ptr)
	{
		m_uLength = (uint)strlen(Ptr);
		memset(m_Ptr, 0, _Cap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}

	TSizedString(const char *Ptr, uint uLength)
	{
		memset(m_Ptr, 0, _Cap);
		memcpy(m_Ptr, Ptr, uLength);
		m_uLength = uLength;
	}

	void Clear()
	{
		memset(m_Ptr, 0, _Cap);
		m_uLength = 0;
	}

	char *Set(const char *Ptr, uint uLength)
	{
		memcpy(m_Ptr, 0, _Cap);
		memcpy(m_Ptr, Ptr, uLength);
		m_uLength = uLength;
	}
	inline char *Set(const char *Ptr) { return Set(Ptr, (uint)strlen(Ptr)); }

	char *Append(const char *Ptr, uint uLength)
	{
		auto toCopy = uLength;
		auto t = m_uLength + toCopy;
		if (t >= _Cap) {
			toCopy = _Cap - 1 - m_uLength;
			t = _Cap - 1;
		}

		auto dst = &m_Ptr[m_uLength];
		memcpy(dst, Ptr, toCopy);
		m_uLength = t;
		return dst;
	}
	inline char *Append(const char *Ptr) { return Append(Ptr, (uint)strlen(Ptr)); }

	char *Find(const char Ch)
	{
		auto c = m_Ptr;
		while (c) {
			if (*c == Ch)
				return c;
			++c;
		}
		return NULL;
	}

	char *FindLast(const char Ch)
	{
		auto c = &m_Ptr[m_uLength];
		while (c != m_Ptr) {
			if (*c == Ch)
				return c;
			--c;
		}
		return NULL;
	}

	operator const char *() { return (const char *)m_Ptr; }
	operator char *() { return m_Ptr; }

	inline uint Length() { return m_uLength; }
	inline char *Get() { return m_Ptr; }

private:
	char m_Ptr[_Cap];
	uint m_uLength;
};

/* -------------------------------------------------------------------------------------------------------------- */
struct String
{
	String()
	{
		m_Ptr = "";
		m_uLength = 0;
		m_uCap = 0;
	}

	String(const char *Ptr, uint Length)
	{
		m_uLength = Length;
		m_uCap = ALIGN16(Length);
		m_Ptr = (char *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, m_uLength);
	}
	String(const char *Ptr) : String(Ptr, (uint)strlen(Ptr)) {}

	~String()
	{
		if (m_Ptr != "") {
			Clear();
		}
	}

	void Clear()
	{
		free(m_Ptr);
		m_Ptr = "";
		m_uLength = 0;
		m_uCap = 0;
	}

	char *Set(const char *Ptr, uint Length)
	{
		if (m_Ptr != "")
			free(m_Ptr);
		m_uLength = Length;
		m_uCap = ALIGN16(Length);
		m_Ptr = (char *)malloc(m_uCap);
		memset(m_Ptr, 0, m_uCap);
		memcpy(m_Ptr, Ptr, Length);
		return m_Ptr;
	}
	char *Set(const char *Ptr) { return Set(Ptr, (uint)strlen(Ptr)); }

	char *Append(const char *Ptr, uint Length)
	{
		char *pDst = nullptr;
		if (m_Ptr != "") {
			auto t = m_uLength + Length;
			if (t > m_uCap) {
				auto oldCap = m_uCap;
				m_uCap = ALIGN16(t);
				m_Ptr = (char *)realloc(m_Ptr, m_uCap);
				memset(&m_Ptr[oldCap], 0, (m_uCap - oldCap));
			}
			pDst = &m_Ptr[m_uLength];
			m_uLength = t;
		}
		else {
			m_uLength = Length;
			m_uCap = ALIGN16(Length);
			m_Ptr = (char *)malloc(m_uCap);
			memset(m_Ptr, 0, m_uCap);
			pDst = m_Ptr;
		}
		memcpy(pDst, Ptr, Length);
		return pDst;
	}
	char *Append(const char *Ptr) { return Append(Ptr, (uint)strlen(Ptr)); }

	char *Find(const char Ch)
	{
		auto c = m_Ptr;
		while (c) {
			if (*c == Ch)
				return c;
			++c;
		}
		return NULL;
	}

	char *FindLast(const char Ch)
	{
		auto c = &m_Ptr[m_uLength];
		while (c != m_Ptr) {
			if (*c == Ch)
				return c;
			--c;
		}
		return NULL;
	}

	inline operator const char *() { return m_Ptr; }
	inline char &operator [](uint it) { return m_Ptr[it]; }
	inline operator void *() { return m_Ptr; }

private:
	char *m_Ptr;
	uint m_uLength;
	uint m_uCap;
};

/* -------------------------------------------------------------------------------------------------------------- */
struct ConstString
{
	ConstString() { m_Ptr = ""; m_uLength = 0; }
	ConstString(const char *Ptr) { m_Ptr = Ptr; m_uLength = (uint)strlen(Ptr); }
	ConstString(const char *Ptr, uint Length) { m_Ptr = Ptr; m_uLength = Length; }

	void Set(const char *Ptr, uint Length)
	{
		m_Ptr = Ptr;
		m_uLength = Length;
	}
	inline void Set(const char *Ptr) { this->Set(Ptr, (uint)strlen(Ptr)); }

	inline operator const char *() { return m_Ptr; }
	inline const char &operator [](uint it) { return m_Ptr[it]; }

	inline void operator =(const char *Ptr) { Set(Ptr); }

private:
	const char *m_Ptr;
	uint m_uLength;
};

#endif // _STL_STRING_H_