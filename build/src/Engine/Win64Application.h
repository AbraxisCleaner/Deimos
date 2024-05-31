#if !defined(_ENGINE_WIN64_APPLICATION_H_)
#define _ENGINE_WIN64_APPLICATION_H_

/*
	Declares the Win64 application state class.
*/

#if !defined(XBR_WIN64)
#	error [__FILE__] should not be included without defining XBR_WIN64.
#endif

#include <pch.h>
#include <Stl/String.h>

namespace Engine
{
	struct Application
	{
		HINSTANCE hInstance;
		TString<TCHAR> CodePath;
	};
}

#endif // _ENGINE_WIN64_APPLICATION_H_