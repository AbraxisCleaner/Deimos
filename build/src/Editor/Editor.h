#if !defined(_EDITOR_STATE_HPP_)
#define _EDITOR_STATE_HPP_

#include <pch.h>
#include <Engine/RhiDx12.h>
#include <Engine/World.h>

struct CEditor : public Engine::CDirect3DWindow
{
	CEditor();
	~CEditor();

	Engine::CDirect3DWindow m_MainWindow;
	Engine::CWorld m_World;
};
extern CEditor *pEditor;

#endif // _EDITOR_STATE_HPP_