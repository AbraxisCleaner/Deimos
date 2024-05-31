#if !defined(_EDITOR_APPLICATION_H_)
#define _EDITOR_APPLICATION_H_

#include <pch.h>
#include <Engine/Direct3D12.h>

struct CEditor
{
	CEditor();
	~CEditor();

private:
	Engine::IRhiWindow m_MainWindow;
};
extern CEditor *gEditor;

#endif // _EDITOR_APPLICATION_H_