#include <pch.h>
#include <Editor/EditorApplication.h>

#if defined(_DEBUG)
int main(int argc, char **argv)
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	CEditor _Editor(GetCommandLine()); // 'Editor' is defined for some reason.
	return 0;
}