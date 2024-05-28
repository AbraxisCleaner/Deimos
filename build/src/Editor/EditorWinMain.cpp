#include <pch.h>
#include <Windows.h>

int main(int argc, char **argv)
{
#if !defined(_DEBUG)
	::FreeConsoleW();
#endif

	return NO_ERROR;
}