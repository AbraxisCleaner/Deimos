#if !defined(_EDITOR_APPLICATION_H_)
#define _EDITOR_APPLICATION_H_

#include <pch.h>
#include <Engine/SystemWin64.h>
#include <Engine/SystemDx12.h>

class CEditorApplication
{
public:
	CEditorApplication() = default;
	CEditorApplication(TCHAR *CmdLine);
	void Shutdown();

private:
	RHI::CWindow m_MainWindow;
};

#endif // _EDITOR_APPLICATION_H_