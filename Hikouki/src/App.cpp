#include "App.h"

rxcpp::subjects::subject<const WPARAM> App::subject;

App::App(HINSTANCE _ins)
	: ins(_ins)
{
	if (!RegisterClassEx(&WNDCLASSEX
		{
			sizeof(WNDCLASSEX),
			0,
			static_cast<WNDPROC>(this->wndProc),
			0,
			0,
			ins,
			LoadIcon((HINSTANCE)NULL, IDI_APPLICATION),
			LoadCursor((HINSTANCE)NULL, IDC_ARROW),
			(HBRUSH)GetStockObject(WHITE_BRUSH),
			0,
			name,
			LoadIcon((HINSTANCE)NULL, IDI_WINLOGO)
		}
	)) throw "Couldn't register window class.";

	if (isFullscreen)
	{
		wh = CreateWindow(
			name,							// �E�B���h�E�N���X�̖��O
			title,							// �^�C�g��
			WS_VISIBLE | WS_POPUP,			// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��
			width, height,				// �E�B���h�E�T�C�Y
			NULL,							// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			ins,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�
	}
	else
	{
		RECT	rWindow, rClient;

		wh = CreateWindow(
			name,							// �E�B���h�E�N���X�̖��O
			title,							// �^�C�g��
			WS_CAPTION | WS_SYSMENU,		// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��(���ƂŒ����Ɉړ������܂�)
			width, height, // �E�B���h�E�T�C�Y
			HWND_DESKTOP,					// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			ins,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�

											// �E�C���h�E�T�C�Y���Čv�Z�iMetrics�����ł́A�t���[���f�U�C���ŃN���C�A���g�̈�T�C�Y���ς���Ă��܂��̂Łj
		GetWindowRect(wh, &rWindow);
		GetClientRect(wh, &rClient);
		auto _width = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + width;
		auto _height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + height;
		SetWindowPos(
			wh,
			NULL,
			GetSystemMetrics(SM_CXSCREEN) / 2 - _width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - _height / 2,
			_width - 1,
			_height - 1,
			SWP_NOZORDER);
	}

	if (!wh) throw "Error";
}

int App::run(int mode)
{
	// �E�C���h�E��\������
	ShowWindow(wh, mode);
	UpdateWindow(wh);

	try
	{
		auto gameController = new GameController(ins, wh, width, height, isFullscreen);
		bool isLoop = true;
		MSG msg;

		subject.get_observable().subscribe([&isLoop](const WPARAM)
		{
			isLoop = false;
		});

		while (isLoop)
		{	// ���b�Z�[�W����[�v
			if (!GetMessage(&msg, NULL, 0, 0))
			{	// ���b�Z�[�W���擾
				break;
			}
			else
			{
				TranslateMessage(&msg); 			// �������b�Z�[�W�ւ̃R���o�[�g�j
				DispatchMessage(&msg); 				// ���b�Z�[�W��WndProc�֑���
			}
		}

		// �Q�[���I���t���O���Z�b�g����
		gameController->SetEnd();

		delete gameController;

		return static_cast<int>(msg.wParam);
	}
	catch (const char *err)
	{
		MessageBox(wh, err, "ERROR", MB_OK);
		return 0;
	}
	catch (std::runtime_error e)
	{
		MessageBox(wh, e.what(), "ERROR", MB_OK);
		return 0;
	}
}

LRESULT App::wndProc(HWND wh, UINT message, WPARAM w, LPARAM l)
{
	switch (message) {
	case WM_KEYDOWN:
		switch (w) {
		case VK_ESCAPE:
			DestroyWindow(wh);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		subject.get_subscriber().on_next(w);
		break;
	default:
		return DefWindowProc(wh, message, w, l);
	}
	return 0;
}
