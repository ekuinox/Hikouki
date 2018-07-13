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
	// �f�o�C�X�I�u�W�F�N�g���쐬
	if (FAILED(input->CreateDevice(GUID_SysKeyboard, &device, NULL)))
		throw "Not found a Keyboard";

	// �f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(device->SetDataFormat(&c_dfDIKeyboard)))
		throw "Failed to set keyboard format data";

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
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
			// �L�[�g���K�[�E�����[�X���𐶐��D
			state_trigger[nCnKey] = (buff[nCnKey] ^ current[nCnKey]) & current[nCnKey];
			state_release[nCnKey] = (buff[nCnKey] ^ current[nCnKey]) & ~current[nCnKey];

			// �L�[�s�[�g���𐶐�
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

			// �L�[�v���X����ۑ�
			buff[nCnKey] = current[nCnKey];
		}
	}
	else
	{
		// �L�[�{�[�h�ւ̃A�N�Z�X�����擾
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
	// �f�o�C�X�I�u�W�F�N�g���쐬
	if (FAILED(input->CreateDevice(GUID_SysMouse, &device, NULL)))
		throw "Not found a mouse";

	// �f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(device->SetDataFormat(&c_dfDIMouse)))
		throw "Failed to set mouse format data";

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	if (FAILED(device->SetCooperativeLevel(wnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		throw "Failed to set mouse cooperative level";


	// �f�o�C�X�̐ݒ�
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// ���Βl���[�h�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j

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

	// �}�E�X�̏�Ԃ��擾���܂�
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
