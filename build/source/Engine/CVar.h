#if !defined(_ENGINE_CVAR_H_)
#define _ENGINE_CVAR_H_

#include <pch.h>

namespace Engine 
{
	template <typename T> 
	struct CVar
	{
		CVar() = delete;
		explicit CVar(const char *sName, T Value) : m_sName(sName), m_Value(Value), m_Default(Value) {}

		T Get() { return m_Value; }
		void Set(T Value) { m_Value = Value; }
		void Reset() { m_Value = m_Default; }

	private:
		const char *m_sName;
		T m_Value;
		T m_Default;
	};
}

#endif // _ENGINE_CVAR_H_
