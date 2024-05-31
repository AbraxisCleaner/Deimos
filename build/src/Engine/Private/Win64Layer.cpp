#include <pch.h>
#include <Engine/Win64Layer.h>

Engine::Win64::SWin64PersistentData _Win64;
Engine::Win64::SWin64PersistentData *gWin64 = &_Win64;



bool Engine::Win64::InitializeLayer(const TCHAR *szCmdLine)
{
	gWin64->hInstance = ::GetModuleHandleW(nullptr);
	gWin64->TransientArena = CStackArena(KILOBYTES(16));



	return true;
}

void Engine::Win64::ShutdownLayer()
{

}