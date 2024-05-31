#include <pch.h>
#include <Stl/String.h>

template <> static uint StrLen<char>(const char *Str) { return (uint)strlen(Str); }
template <> static uint StrLen<wchar_t>(const wchar_t *Str) { return (uint)wcslen(Str); }
template <> static bool StrCmp<char>(const char *Left, const char *Right) { return !strcmp(Left, Right); }
template <> static bool StrCmp<wchar_t>(const wchar_t *Left, const wchar_t *Right) { return !wcscmp(Left, Right); }