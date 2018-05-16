#pragma once

#include <cstdio>
#include <Windows.h>
#include <iostream>

class DebugConsole {
	FILE *out;
	FILE *in;

	DebugConsole();
	bool init();
	void uninit();
public:
	~DebugConsole();
	static void create_console_window();
	static void message(const char* str);
};
