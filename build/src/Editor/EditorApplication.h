#if !defined(_EDITOR_APPLICATION_H_)
#define _EDITOR_APPLICATION_H_

#include <pch.h>
#include <Engine/SystemWin64.h>
#include <Engine/SystemDx12.h>

class CEditor
{
public:
	CEditor() = default;
	CEditor(TCHAR *CmdLine);
	void Shutdown();

private:
	System::CWindow m_MainWindow;
};

#endif // _EDITOR_APPLICATION_H_