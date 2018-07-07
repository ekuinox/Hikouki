#pragma once

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#include <dinput.h>
#include "KeyCode.h"

class Input
{
private:
	LPDIRECTINPUT8 input;
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
public:
	Keyboard *keyboard;
	Mouse *mouse;

	Input(HINSTANCE, HWND);
	~Input();
	void update();
	bool getPress(int key) { return keyboard->getPress(key); }
	bool getTrigger(int key) { return keyboard->getTrigger(key); }
	bool getRepeat(int key) { return keyboard->getRepeat(key); }
	bool getRelease(int key) { return keyboard->getRelease(key); }
	bool getPress(KeyCode key) { return keyboard->getPress(static_cast<int>(key)); }
	bool getTrigger(KeyCode key) { return keyboard->getTrigger(static_cast<int>(key)); }
	bool getRepeat(KeyCode key) { return keyboard->getRepeat(static_cast<int>(key)); }
	bool getRelease(KeyCode key) { return keyboard->getRelease(static_cast<int>(key)); }
	DIMOUSESTATE getMouseState() { return mouse->getState(); }
};
