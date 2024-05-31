#include <pch.h>
#include <Engine/SystemWin64.h>
#include <Engine/SystemDx12.h>

int EditorWindowSize[2] = { 1280, 720 };


bool InitializeEditor()
{
	System::Win64::Initialize();
	System::Dx12::Initialize();
	return true;
}

void ShutdownEditor()
{
	System::Dx12::Shutdown();
	System::Win64::Shutdown();
}


#if defined(_DEBUG)
int main(int argc, char **argv)
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	InitializeEditor();
	System::CWindow MainWindow(1280, 720, L"Editor");

	MainWindow.Show();
	while (!MainWindow.ShouldClose()) 
	{
		System::CWindow::PumpEvents();
	}

	MainWindow.Release();
	ShutdownEditor();
	return 0;
}