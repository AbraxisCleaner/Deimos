#include <pch.h>
#include <Editor/EditorApplication.h>

#if defined(_DEBUG)
int main(int argc, char **argv)
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	SEditorCreateInfo EditorInfo = {};
	EditorInfo.iWidth = 1280;
	EditorInfo.iHeight = 720;

#ifdef _DEBUG
	char *pCmdLine = GetCommandLineA();
#endif

	auto c = pCmdLine;
	while (*c) {
		if (!strcmp(c, "-help")) {
			printf(
				"---- Valid Switches ----\n"
				"\t-width *\n"
				"\t-height *\n"
				"\t-forceDebug | Force vulkan validation.\n"
				"\t-allowConsole | Allow the dev console to be used with '~'.\n"
			);
			return NO_ERROR;
		}
		else if (!strcmp(c, "-width")) {
			while (*c != ' ') ++c;
			while (*c == ' ') ++c;
			EditorInfo.iWidth = atoi(c);
		} 
		else if (!strcmp(c, "-height")) {
			while (*c != ' ') ++c;
			while (*c == ' ') ++c;
			EditorInfo.iHeight = atoi(c);
		}
		else if (!strcmp(c, "-forceDebug")) {
			EditorInfo.iCmdLineFlags |= EDITOR_FLAG_FORCE_VK_DEBUGGING;
		}
		else if (!strcmp(c, "-allowConsole")) {
			EditorInfo.iCmdLineFlags |= EDITOR_FLAG_ALLOW_GAME_CONSOLE;
		}
		while (*c && *c != ' ') ++c;
		if (!*c)
			break;
		++c;
	}

	CEditorApplication EditorApp;
	gEditor = &EditorApp;

	if (EditorApp.Initialize(EditorInfo)) {

		EditorApp.ShowWindow();

		while (!EditorApp.ShouldClose()) {
			EditorApp.Tick();
		}

		EditorApp.HideWindow();
		EditorApp.Release();
	}

	return NO_ERROR;
}