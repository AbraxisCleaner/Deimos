#if !defined(_ENGINE_INI_H_)
#define _ENGINE_INI_H_

#include <pch.h>
#include <Stl/Container.h>
#include <Stl/String.h>

namespace Engine 
{
	struct SIniVar
	{
		struct 
		{
			char *Ptr;
			uint uLength;
		} Name;
		union 
		{
			int iValue;
			float fValue;
			char *sValue;
		};
	};

	struct CIniGroup
	{
		ConstString m_Name;
		TMap<SIniVar> m_Vars;
	};

	struct CIniConfig
	{
		TMap<CIniGroup> m_Groups;
	};
}

#endif // _ENGINE_INI_H_