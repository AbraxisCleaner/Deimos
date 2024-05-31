#if !defined(_ENGINE_CVAR_H_)
#define _ENGINE_CVAR_H_

#include <pch.h>

namespace Engine 
{
	struct CVar
	{
		CVar() = delete;
		explicit CVar(const char *sName, const char *sValue) { m_sName = sName; m_sValue = sValue; }
		explicit CVar(const char *sName, int iValue) { m_sName = sName; m_iValue = iValue; }
		explicit CVar(const char *sName, float fValue) { m_sName = sName; m_fValue = fValue; }
		explicit CVar(const char *sName, bool bValue) { m_sName = sName; m_bValue = bValue; }

		template <typename T> T Get();
		template <> const char *Get<const char *>() { return m_sValue; }
		template <> int Get<int>() { return m_iValue; }
		template <> float Get<float>() { return m_fValue; }
		template <> bool Get<bool>() { return m_bValue; }

		template <typename T> void Set(T);

	private:
		const char *m_sName;
		union {
			const char *m_sValue;
			int m_iValue;
			float m_fValue;
			bool m_bValue;
		};
	};
}

#endif // _ENGINE_CVAR_H_
