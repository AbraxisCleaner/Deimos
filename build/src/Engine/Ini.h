#if !defined(_ENGINE_INI_H_)
#define _ENGINE_INI_H_

#include <pch.h>
#include <Stl/String.h>

namespace Engine
{
	TStaticString<char> IniReadString(char *pSource, const char *sGroup, const char *sName);
	int IniReadInteger(char *pSource, const char *sGroup, const char *sName);
	float IniReadFloat(char *pSource, const char *sGroup, const char *sName);
	bool IniReadBoolean(char *pSource, const char *sGroup, const char *sName);
}

#endif // _ENGINE_INI_H_