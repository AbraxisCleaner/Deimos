#if !defined(_ENGINE_LIBRARY_H_)
#define _ENGINE_LIBRARY_H_

#include <pch.h>

#if defined(XBR_WIN64)
#	include <libloaderapi.h>
#	define SysLoadLibrary(sName) LoadLibraryA(sName ## ".lib")
#	define SysGetProcAddr(Lib, sName) ::GetProcAddress(lib, sName)
#	define SysFreeLibrary(Lib) ::FreeLibrary(Lib)
#else
#endif

#endif // _ENGINE_LIBRARY_H_