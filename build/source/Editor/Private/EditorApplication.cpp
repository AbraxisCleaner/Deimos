#include <pch.h>
#include <Editor/EditorApplication.h>

CEditorApplication::CEditorApplication(TCHAR *CmdLine)
{
	System::Initialize();
	RHI::Dx12::Initialize();

	m_MainWindow = RHI::CWindow(1280, 720, STR("Editor"));
	m_MainWindow.Show();

	while (true) 
	{
		RHI::CWindow::PumpEvents();

		if (m_MainWindow.ShouldClose())
			break;
	}

	Shutdown();
}

void CEditorApplication::Shutdown()
{
	RHI::Dx12::Shutdown();
	System::Shutdown();
}