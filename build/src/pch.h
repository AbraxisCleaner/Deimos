#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <new.h>

#define ZeroThat(x) memset(x, 0, sizeof(*x))
#define ZeroThis() memset(this, 0, sizeof(*this))

#define ALIGN(x, y) ((x + (y - 1)) & ~(y - 1))
#define ALIGN8(x) ALIGN(x, 8)
#define ALIGN16(x) ALIGN(x, 16)

#define KILOBYTES(x) (x * 1024)
#define MEGABYTES(x) (x * 1024 * 1024)
#define GIGABYTES(x) (x * 1024 * 1024 * 1024)

#define IS_NUMBER(c) (c >= 0 && c <= 9)
#define IS_LETTER(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define IS_WHITESPACE(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

#if defined(_MSC_VER)
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#else
#endif

#define LSTR(x) L ## x
#if !defined(XBR_FORCE_MBC)
#	define STR(x) LSTR(x)
#	define TCHAR wchar_t
typedef const wchar_t *str_t;
#else
#	define STR(x) x
#	define TCHAR char
#	define MBCS
typedef const char *str_t;
#endif
#define _TCHAR_DEFINED

typedef unsigned int uint;
typedef unsigned char uchar;

typedef long long s64;
typedef long s32;
typedef short s16;
typedef char s8;
typedef unsigned long long u64;
typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef float f32;
typedef double f64;

typedef s64 b64;
typedef s32 b32;
typedef s16 b16;
typedef s8 b8;
