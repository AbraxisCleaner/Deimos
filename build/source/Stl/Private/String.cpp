#include <pch.h>
#include <Stl/String.h>

template <> static uint TStrlen<char>(const char *Str) { return (uint)strlen(Str); }
template <> static uint TStrlen<wchar_t>(const wchar_t *Str) { return (uint)wcslen(Str); }

template <> static bool TStrcmp<char>(const char *Left, const char *Right) { return !strcmp(Left, Right); }
template <> static bool TStrcmp<wchar_t>(const wchar_t *Left, const wchar_t *Right) { return !wcscmp(Left, Right); }

template <> const char *TEmptyString() { return ""; }
template <> const wchar_t *TEmptyString() { return L""; }