#pragma once

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#include <dinput.h>

class Input {
private:
	LPDIRECTINPUT8 input;
public:
	Input(HINSTANCE);
	~Input();
	LPDIRECTINPUT8 get();
};

class Keyboard {
private:
	static constexpr unsigned int NUM_KEY_MAX = 256;
	static constexpr unsigned int LIMIT_COUNT_REPEAT = 20;
	LPDIRECTINPUTDEVICE8 device;
	BYTE buff[NUM_KEY_MAX];
	BYTE state_trigger[NUM_KEY_MAX];
	BYTE state_repeat[NUM_KEY_MAX];
	BYTE state_release[NUM_KEY_MAX];
	unsigned int repeat_cnt[NUM_KEY_MAX];
public:
	Keyboard(LPDIRECTINPUT8, HWND);
	~Keyboard();
	void update();
	bool getPress(int);
	bool getTrigger(int);
	bool getRepeat(int);
	bool getRelease(int);
};

class Mouse {
private:
	LPDIRECTINPUTDEVICE8 device;
	DIMOUSESTATE buff;
public:
	Mouse(LPDIRECTINPUT8, HWND);
	~Mouse();
	void update();
	DIMOUSESTATE getState();
	
};