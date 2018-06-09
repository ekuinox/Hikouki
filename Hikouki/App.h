#pragma once

#pragma comment(lib, "d3d9.lib")			// リンク対象ライブラリにd3d9.libを追加
#pragma comment(lib, "d3dx9.lib")			// リンク対象ライブラリにd3dx9.libを追加
#pragma comment(lib, "winmm.lib")			// リンク対象ライブラリにwinmm.libを追加

#include <windows.h>
#include "GameController.h"

class App
{
private:
	static constexpr const char* title = "Title";
	static constexpr const char* name = "name";
	static constexpr unsigned width = 1280;
	static constexpr unsigned height = 720;
	static constexpr bool isFullscreen = false;
	HWND wh;
	HINSTANCE ins;
	static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
public:
	App(HINSTANCE);
	int run(int);
};