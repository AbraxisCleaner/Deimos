#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <new.h>

#define ZeroThat(x) memset(x, 0, sizeof(*x))
#define ZeroThis() memset(this, 0, sizeof(*this))

#define ALIGN(x, y) ((x + (y - 1)) & ~(y - 1))
#define ALIGN8(x) ALIGN(x, 8)
#define ALIGN16(x) ALIGN((x), 16)

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
#else
#	define STR(x) x
#	define TCHAR char
#	define MBCS
#endif
#define _TCHAR_DEFINED

typedef unsigned int uint;
typedef unsigned char uchar;

typedef long long int64;
typedef long int32;
typedef short int16;
typedef char int8;
typedef unsigned long long uint64;
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef float float32;
typedef double float64;

typedef int64 bool64;
typedef int32 bool32;
typedef int16 bool16;
typedef int8 bool8;
