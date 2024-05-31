#include <pch.h>
#include <Engine/ConfigFile.h>
#include <Engine/FileSystem.h>

Engine::CIniConfig::CIniConfig(const TCHAR *Path)
{
	size_t SourceLength;
	m_StaticSource = ReadEntireFile(Path, &SourceLength);

	char *c = (char *)m_StaticSource;
	auto tmp = c;

	// Count the number of groups.
	uint num = 0;

	while (*c) {
		if (*c == '[') {
			++num;
		}
		++c;
	}

	m_Groups.Reserve(num);

	// Reserve the number of 
	uint currGroup = -1;
	num = 0;
	c = (char *)m_StaticSource;

	while (*c) {
		if (*c == '[') {
			if (currGroup != (uint)-1) {
				m_Groups[currGroup].m_Vars.Reserve(num);
				num = 0;
			}
			++currGroup;
		}
		else if (*c == '=') {
			++num;
		}
		++c;
	}

	// Do the actual parsing.
	c = (char *)m_StaticSource;
	currGroup = (uint)-1;
	num = 0;

	while (*c) {
		switch (*c) {
		case '[': {
			++currGroup;

			tmp = (c + 1);
			while (*c != ']') ++c;
			m_Groups[currGroup].m_Name = TStaticString<TCHAR>(tmp, (uint)(c - tmp));

			num = 0;
		} break;

		case '=': {
			auto pVar = &(m_Groups[currGroup].m_Vars[num]);

			--c;
			while (IS_WHITESPACE(*c)) --c;
			tmp = c + 1;
			while (!IS_WHITESPACE(*c)) --c;
			++c;
			pVar->Name = TStaticString<TCHAR>(c, (uint)(tmp - c));

			while (*c != '=') ++c;
			++c;
			while (IS_WHITESPACE(*c)) ++c;
			tmp = c;

			pVar->Type = eVarInteger;

			while (!IS_WHITESPACE(*c)) {
				if (IS_LETTER(*c) && pVar->Type == eVarInteger) {
					pVar->Type = eVarString;
				}
				else if (*c == '.' && pVar->Type == eVarInteger) {
					pVar->Type = eVarFloat;
				}
				++c;
			}

			switch (pVar->Type) {
			case eVarInteger: { pVar->iValue = atoi(tmp); } break;
			case eVarFloat: { pVar->iFloat = (float)atof(tmp); } break;
			case eVarString: {
				if (StrCmp<char>(tmp, "true") || StrCmp<char>(tmp, "True"))
					pVar->bValue = true;
				else if (StrCmp<char>(tmp, "false") || StrCmp<char>(tmp, "False"))
					pVar->bValue = false;
				else
					pVar->sValue = TStaticString<char>(tmp, (uint)(c - tmp));
			} break;
			}
		} break;
		}
		++c;
	}
}