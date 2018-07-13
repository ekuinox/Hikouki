#include "Input.h"

Input::Input(HINSTANCE ins, HWND wnd)
{
	if (FAILED(DirectInput8Create(ins, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&input, NULL)))
	{
		throw "Init Input Error";
	}

	mouse = new Mouse(input, wnd);
	keyboard = new Keyboard(input, wnd);
}

Input::~Input()
{
	if (input) input->Release();
	delete mouse;
	delete keyboard;
}

void Input::update()
{
	mouse->update();
	keyboard->update();
}

Input::Keyboard::Keyboard(LPDIRECTINPUT8 input, HWND wnd)
{
	// デバイスオブジェクトを作成
	if (FAILED(input->CreateDevice(GUID_SysKeyboard, &device, NULL)))
		throw "Not found a Keyboard";

	// データフォーマットを設定
	if (FAILED(device->SetDataFormat(&c_dfDIKeyboard)))
		throw "Failed to set keyboard format data";

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(device->SetCooperativeLevel(wnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		throw "Failed to set keyboard cooperative level";

	device->Acquire();
}

Input::Keyboard::~Keyboard()
{
	if (device)
	{
		device->Unacquire();
		device->Release();
	}
}

void Input::Keyboard::update()
{
	BYTE current[NUM_KEY_MAX];

	if (SUCCEEDED(device->GetDeviceState(sizeof(current), current)))
	{
		for (int nCnKey = 0; nCnKey < NUM_KEY_MAX; nCnKey++)
		{
			// キートリガー・リリース情報を生成．
			state_trigger[nCnKey] = (buff[nCnKey] ^ current[nCnKey]) & current[nCnKey];
			state_release[nCnKey] = (buff[nCnKey] ^ current[nCnKey]) & ~current[nCnKey];

			// キーピート情報を生成
			if (current[nCnKey])
			{
				if (repeat_cnt[nCnKey] < LIMIT_COUNT_REPEAT)
				{
					repeat_cnt[nCnKey]++;
					if (repeat_cnt[nCnKey] == 1
						|| repeat_cnt[nCnKey] >= LIMIT_COUNT_REPEAT)
					{
						state_repeat[nCnKey] = current[nCnKey];
					}
					else
					{
						state_repeat[nCnKey] = 0;
					}
				}
			}
			else
			{
				repeat_cnt[nCnKey] = 0;
				state_repeat[nCnKey] = 0;
			}

			// キープレス情報を保存
			buff[nCnKey] = current[nCnKey];
		}
	}
	else
	{
		// キーボードへのアクセス権を取得
		device->Acquire();
	}
}

bool Input::Keyboard::getPress(int key)
{
	return (buff[key] & 0x80) ? true : false;
}

bool Input::Keyboard::getTrigger(int key)
{
	return (state_trigger[key] & 0x80) ? true : false;
}

bool Input::Keyboard::getRepeat(int key)
{
	return (state_repeat[key] & 0x80) ? true : false;
}

bool Input::Keyboard::getRelease(int key)
{
	return (state_release[key] & 0x80) ? true : false;
}

Input::Mouse::Mouse(LPDIRECTINPUT8 input, HWND wnd)
{
	// デバイスオブジェクトを作成
	if (FAILED(input->CreateDevice(GUID_SysMouse, &device, NULL)))
		throw "Not found a mouse";

	// データフォーマットを設定
	if (FAILED(device->SetDataFormat(&c_dfDIMouse)))
		throw "Failed to set mouse format data";

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(device->SetCooperativeLevel(wnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		throw "Failed to set mouse cooperative level";


	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

	if (FAILED(device->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
		throw "Failed to set mouse prop";

	device->Acquire();
}

Input::Mouse::~Mouse()
{
	if (device)
	{
		device->Unacquire();
		device->Release();
	}
}

void Input::Mouse::update()
{
	/*
	DIMOUSESTATE current;

	// マウスの状態を取得します
	HRESULT	hr = device->GetDeviceState(sizeof(DIMOUSESTATE), &current);
	if (hr == DIERR_INPUTLOST) {
		device->Acquire();
		hr = device->GetDeviceState(sizeof(DIMOUSESTATE), &current);
	}

	if (memcmp(&buff, &current, sizeof(DIMOUSESTATE)) != 0) {
		memcpy(&buff, &current, sizeof(DIMOUSESTATE));
	}
	*/
	HRESULT hr;
	hr = device->Acquire();
	if (FAILED(hr))
	{
		switch (hr) {
		case DIERR_INVALIDPARAM:
			throw "DIERR_INVALIDPARAM";
			break;
		case DIERR_NOTINITIALIZED:
			throw "DIERR_NOTINITIALIZED";
			break;
		case DIERR_OTHERAPPHASPRIO:
			throw "DIERR_OTHERAPPHASPRIO";
			break;
		}
	}
	hr = device->GetDeviceState(sizeof(DIMOUSESTATE), &buff);
	if (FAILED(hr))
	{
		switch (hr)
		{
		case DIERR_INPUTLOST:
			throw "DIERR_INPUTLOST";
			break;
		case DIERR_INVALIDPARAM:
			throw "DIERR_INVALIDPARAM";
			break;
		case DIERR_NOTACQUIRED:
			throw "DIERR_NOTACQUIRED";
			break;
		case DIERR_NOTINITIALIZED:
			throw "DIERR_NOTINITIALIZED";
			break;
		case E_PENDING:
			throw "E_PENDING";
			break;
		}
	}
}

DIMOUSESTATE Input::Mouse::getState()
{
	return buff;
}
