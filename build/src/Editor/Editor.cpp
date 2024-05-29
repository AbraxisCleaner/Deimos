#include <pch.h>
#include <Editor/Editor.h>

CEditor _editor;
CEditor *pEditor = &_editor;


int main(int argc, char **argv)
{
#if !defined(_DEBUG)
	::FreeConsoleW();
#endif

	return NO_ERROR;
}


CEditor::CEditor()
{
}

CEditor::~CEditor()
{
}