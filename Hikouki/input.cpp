#include "input.h"

Input::Input(HINSTANCE ins)
{
	if (FAILED(DirectInput8Create(ins, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&input, NULL)))
	{
		throw "Init Input Error";
	}
}

Input::~Input()
{
	if (input) input->Release();
}

LPDIRECTINPUT8 Input::get()
{
	return input;
}

Keyboard::Keyboard(LPDIRECTINPUT8 input, HWND wnd)
{
	// デバイスオブジェクトを作成
	if (FAILED(input->CreateDevice(GUID_SysKeyboard, &device, NULL)))
		throw "Not found a Keyboard";

	// データフォーマットを設定
	if (FAILED(device->SetDataFormat(&c_dfDIKeyboard)))
		throw "Failed to format keyboard data";

	// 強調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(device->SetCooperativeLevel(wnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		throw "キーボードの協調モードを設定できなかった．";

	device->Acquire();
}

Keyboard::~Keyboard()
{
	if (device)
	{
		device->Unacquire();
		device->Release();
	}
}

void Keyboard::update()
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

bool Keyboard::getPress(int key)
{
	return (buff[key] & 0x80) ? true : false;
}

bool Keyboard::getTrigger(int key)
{
	return (state_trigger[key] & 0x80) ? true : false;
}

bool Keyboard::getRepeat(int key)
{
	return (state_repeat[key] & 0x80) ? true : false;
}

bool Keyboard::getRelease(int key)
{
	return (state_release[key] & 0x80) ? true : false;
}

