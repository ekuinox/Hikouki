#include "App.h"

auto APIENTRY WinMain(HINSTANCE ins, HINSTANCE, LPSTR, int mode) -> int
{
	try
	{
		return (new App(ins))->run(mode);
	}
	catch (...)
	{
		return -1;
	}
}