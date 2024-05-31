#if !defined(_EDITOR_CONFIG_FILE_H_)
#define _EDITOR_CONFIG_FILE_H_

#include <pch.h>
#include <Stl/Container.h>
#include <Stl/String.h>

namespace Engine
{
	enum EVarType
	{
		eVarNone,
		eVarInteger,
		eVarFloat,
		eVarBoolean,
		eVarString,
		eVarStruct,
		eVarArray,
	};

	struct SIniVar 
	{
		TStaticString<char> Name;
		EVarType Type;
		union {
			int iValue;
			bool bValue;
			float fValue;
			TStaticString<char> sValue;
		};

		inline operator int &() { return iValue; }
		inline operator bool &() { return bValue; }
		inline operator float &() { return fValue; }
	};

	struct SIniGroup
	{
		TStaticString<char> m_Name;
		TArray<SIniVar> m_Vars;

		SIniVar &GetVar(const char *Name);
	};

	struct CIniConfig
	{
		CIniConfig() = default;
		CIniConfig(const TCHAR *Path);

		void Release();
		void Reload();
		SIniGroup &GetGroup(const char *Name);
		SIniVar &GetVar(const char *GroupName, const char *VarName);

	private:
		TStaticString<TCHAR> m_Path;
		TArray<SIniGroup> m_Groups;
		void *m_StaticSource;
	};

	/* -------------------------------------------------- */
	struct CNewIniConfig
	{
		CNewIniConfig() = default;
		CNewIniConfig(const TCHAR *FilePath);
		~CNewIniConfig() = default;

		void LoadFile(const TCHAR *FilePath);
		void LoadSource(char *pSource, size_t SourceLength);

	private:
		TString<char> m_Source;
	};
}

#endif // _EDITOR_CONFIG_FILE_H_