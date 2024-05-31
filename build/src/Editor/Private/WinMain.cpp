#include <pch.h>
#include <Engine/Win64Application.h>
#include <Engine/RhiDX12.h>


#if defined(_DEBUG)
int main(int argc, char **argv)
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	return 0;
}