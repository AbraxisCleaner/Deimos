#include <pch.h>
#include <Engine/Win64Layer.h>
#include <Engine/Dx12RHI.h>


#if defined(_DEBUG)
int main(int argc, char **argv)
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	Engine::Win64::Initialize(::GetCommandLine());
	Engine::Dx12::Initialize();

	Engine::Dx12::CWindow MainWindow(1280, 720, L"Editor");
	while (!MainWindow.ShouldClose()) {
	}

	Engine::Dx12::Shutdown();
	Engine::Win64::Shutdown();
	return 0;
}