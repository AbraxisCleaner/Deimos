#include <pch.h>
#include <Engine/Ini.h>

TStaticString<char> Engine::IniReadString(char *pSource, const char *sGroup, const char *sName)
{
	char *c = pSource;
	char *tmp = c;

	while (*c) {
		if (*c == '[') {
			tmp = (c + 1);
			while (*c != ']') ++c;
			
			if (StrCmp(tmp, sGroup)) {
				while (*c && *c != '[') {
					if (*c == '=') {
						--c;
						while (IS_WHITESPACE(*c)) --c;
						tmp = (c + 1);
						while (!IS_WHITESPACE(*c)) --c;
						++c;

						if (StrCmp(c, sName)) {
							while (*c != '=') ++c;
							++c;
							while (IS_WHITESPACE(*c)) ++c;
							tmp = c;
							while (!IS_WHITESPACE(*c)) ++c;
							return TStaticString<char>(tmp, (uint)(c - tmp)); // Success
						}
					}
					++c;
				}
				return TStaticString<char>(); // Fail
			}
		}
		++c;
	}
	return TStaticString<char>(); // Fail
}

int Engine::IniReadInteger(char *pSource, const char *sGroup, const char *sName)
{
	auto c = pSource;
	auto tmp = c;

	while (*c) {
		if (*c == '[') {
			tmp = (c + 1);
			while (*c != ']') ++c;

			if (StrCmp(tmp, sGroup)) {
				while (*c && *c != '[') {
					if (*c == '=') {
						--c;
						while (IS_WHITESPACE(*c)) --c;
						tmp = (c + 1);
						while (!IS_WHITESPACE(*c)) --c;
						++c;

						if (StrCmp(c, sName)) {
							while (*c != '=') ++c;
							++c;
							while (IS_WHITESPACE(*c)) ++c;
							return atoi(c); // Success
						}
					}
					++c;
				}
				return 0; // Fail
			}
		}
		++c;
	}
	return 0; // Fail
}

float Engine::IniReadFloat(char *pSource, const char *sGroup, const char *sName)
{
	auto c = pSource;
	auto tmp = c;

	while (*c) {
		if (*c == '[') {
			tmp = (c + 1);
			while (*c != ']') ++c;

			if (StrCmp(tmp, sGroup)) {
				while (*c && *c != '[') {
					if (*c == '=') {
						--c;
						while (IS_WHITESPACE(*c)) --c;
						tmp = (c + 1);
						while (!IS_WHITESPACE(*c)) --c;
						++c;

						if (StrCmp(c, sName)) {
							while (*c != '=') ++c;
							++c;
							while (IS_WHITESPACE(*c)) ++c;
							return (float)atof(c); // Success
						}
					}
					++c;
				}
				return 0; // Fail
			}
		}
		++c;
	}
	return 0; // Fail
}

bool Engine::IniReadBoolean(char *pSource, const char *sGroup, const char *sName)
{
	auto c = pSource;
	auto tmp = c;

	while (*c) {
		if (*c == '[') {
			tmp = (c + 1);
			while (*c != ']') ++c;

			if (StrCmp(tmp, sGroup)) {
				while (*c && *c != '[') {
					if (*c == '=') {
						--c;
						while (IS_WHITESPACE(*c)) --c;
						tmp = (c + 1);
						while (!IS_WHITESPACE(*c)) --c;
						++c;

						if (StrCmp(c, sName)) {
							while (*c != '=') ++c;
							++c;
							while (IS_WHITESPACE(*c)) ++c;

							if (StrCmp(c, "true") || StrCmp(c, "True"))
								return true; // Success
							else if (StrCmp(c, "false") || StrCmp(c, "False"))
								return false; // Success
							else
								return false; // Failure
						}
					}
					++c;
				}
				return 0; // Fail
			}
		}
		++c;
	}
	return 0; // Fail
}