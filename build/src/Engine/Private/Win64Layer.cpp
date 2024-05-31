#include <pch.h>
#include <Engine/Win64Layer.h>
#include <Engine/FileSystem.h>
#include <Engine/Ini.h>
#include <Engine/DebugLog.h>

#define STB_SPRINTF_IMPLEMENTATION
#include <../vendor/stb_sprintf.h>

Engine::Win64::SWin64PersistentData _Win64;
Engine::Win64::SWin64PersistentData *gWin64 = &_Win64;

Engine::CVar<int> Engine::gTransientMemoryKb("iTransientMemorySizeKb", 32);

bool Engine::Win64::Initialize(TCHAR *szCmdLine)
{
	gWin64->hInstance = ::GetModuleHandleW(nullptr);

	// -- Load Ini
	HANDLE F = ::CreateFile(STR("Editor.ini"), GENERIC_READ, 0x01, nullptr, OPEN_EXISTING, 128, nullptr);
	VERIFY(F != (HANDLE)-1);

	LARGE_INTEGER Li;
	::GetFileSizeEx(F, &Li);
	
	char *pIni = (char *)malloc(Li.QuadPart + 1);
	pIni[Li.QuadPart] = 0;
	::ReadFile(F, pIni, Li.LowPart, nullptr, nullptr);

	::CloseHandle(F);

	// -- Set Ini configs.
	int iTransientMemoryKb = IniReadInteger(pIni, "Engine", "iTransientMemoryKb");

	if (iTransientMemoryKb)
		gTransientMemoryKb.Set(iTransientMemoryKb);

	// -- Initialization.
	gWin64->TransientArena = CStackArena(KILOBYTES(gTransientMemoryKb.Get()));

	// -- Cleanup.
	free(pIni);

	return true;
}

void Engine::Win64::Shutdown()
{

}