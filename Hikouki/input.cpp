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
	// �f�o�C�X�I�u�W�F�N�g���쐬
	if (FAILED(input->CreateDevice(GUID_SysKeyboard, &device, NULL)))
		throw "Not found a Keyboard";

	// �f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(device->SetDataFormat(&c_dfDIKeyboard)))
		throw "Failed to format keyboard data";

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	if (FAILED(device->SetCooperativeLevel(wnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		throw "�L�[�{�[�h�̋������[�h��ݒ�ł��Ȃ������D";

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

