//*****************************************************************************
//!	@file	main.cpp
//!	@brief	
//!	@note	GM31�ЂȌ`���C��
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <thread>
#include "GameController.h"

//-----------------------------------------------------------------------------
// �}�N���̒�`
//-----------------------------------------------------------------------------
#define		NAME			"win32A"
#define		TITLE			"GM31 �ЂȂ���"
#define		SCREEN_X		1280
#define		SCREEN_Y		720
#define		FULLSCREEN		0				// �t���X�N���[���t���O

//-----------------------------------------------------------------------------
// �����N���C�u�����̐ݒ�
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")			// �����N�Ώۃ��C�u������d3d9.lib��ǉ�
#pragma comment(lib, "d3dx9.lib")			// �����N�Ώۃ��C�u������d3dx9.lib��ǉ�
#pragma comment(lib, "winmm.lib")			// �����N�Ώۃ��C�u������winmm.lib��ǉ�

//-----------------------------------------------------------------------------
// �v���g�^�C�v�錾
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);			// �E�C���h�E�v���V�[�W��
void	CALLBACK TimerProc(UINT ,UINT,DWORD,DWORD,DWORD);			// �^�C�}����

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
int				g_timerid=0;		// �^�C�}�h�c
GameController *game_controller;

//==============================================================================
//!	@fn		WinMain
//!	@brief	�G���g���|�C���g
//!	@param	�C���X�^���X�n���h��
//!	@param	�Ӗ��Ȃ�
//!	@param	�N�����̈���������
//!	@param	�E�C���h�E�\�����[�h
//!	@retval	TRUE�@�����I��/FALSE�@���s�I��
//!	@note	
//==============================================================================
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// �A�v���P�[�V�����̃n���h��
					 HINSTANCE 	hPrevInstance,	// ����Windows�o�[�W�����ł͎g���Ȃ�
					 LPSTR 		lpszArgs, 		// �N�����̈����i������j
					 int 		nWinMode)		// �E�C���h�E�\�����[�h
{
	HWND			hwnd;						// �E�C���h�E�n���h��
	MSG				msg;						// ���b�Z�[�W�\����
	WNDCLASSEX		wcex;						// �E�C���h�E�N���X�\����
	int				width = SCREEN_X;			// �E�C���h�E�̕� �v�Z�p���[�N
	int				height = SCREEN_Y;			// �E�C���h�E�̍��� �v�Z�p���[�N

	// �E�C���h�E�N���X���̃Z�b�g
	wcex.hInstance		= hInstance;			// �C���X�^���X�l�̃Z�b�g
	wcex.lpszClassName	= NAME;					// �N���X��
	wcex.lpfnWndProc	= (WNDPROC)WndProc;		// �E�C���h�E���b�Z�[�W�֐�
	wcex.style			= 0;					// �E�C���h�E�X�^�C��
	wcex.cbSize 		= sizeof(WNDCLASSEX);	// �\���̂̃T�C�Y
	wcex.hIcon			= LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ���[�W�A�C�R��
	wcex.hIconSm		= LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// �X���[���A�C�R��
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// �J�[�\���X�^�C��
	wcex.lpszMenuName	= 0; 					// ���j���[�Ȃ�
	wcex.cbClsExtra		= 0;					// �G�L�X�g���Ȃ�
	wcex.cbWndExtra		= 0;					
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);		// �w�i�F��

	if (!RegisterClassEx(&wcex)) return FALSE;	// �E�C���h�E�N���X�̓o�^

	if(FULLSCREEN){
		hwnd = CreateWindow(
			NAME,							// �E�B���h�E�N���X�̖��O
			TITLE,							// �^�C�g��
			WS_VISIBLE | WS_POPUP,			// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��
			SCREEN_X, SCREEN_Y,				// �E�B���h�E�T�C�Y
			NULL,							// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			hInstance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�
	}else{
		RECT	rWindow, rClient;

		hwnd = CreateWindow(
			NAME,							// �E�B���h�E�N���X�̖��O
			TITLE,							// �^�C�g��
			WS_CAPTION | WS_SYSMENU,		// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��(���ƂŒ����Ɉړ������܂�)
			SCREEN_X, SCREEN_Y,				// �E�B���h�E�T�C�Y
			HWND_DESKTOP,					// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			hInstance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�
		
		// �E�C���h�E�T�C�Y���Čv�Z�iMetrics�����ł́A�t���[���f�U�C���ŃN���C�A���g�̈�T�C�Y���ς���Ă��܂��̂Łj
		GetWindowRect(hwnd, &rWindow);
		GetClientRect(hwnd, &rClient);
		width  = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + SCREEN_X;
		height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + SCREEN_Y;
		SetWindowPos(
			hwnd,
			NULL,
			GetSystemMetrics(SM_CXSCREEN)/2-width/2,
			GetSystemMetrics(SM_CYSCREEN)/2-height/2,
			width-1,
			height-1,
			SWP_NOZORDER);
	}

	if (!hwnd) return FALSE;

	// �E�C���h�E��\������
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	try{
		game_controller = new GameController(hInstance, hwnd, SCREEN_X, SCREEN_Y, FULLSCREEN);
	} catch (const char *err) {
		MessageBox(hwnd, "ERROR", err, MB_OK);
	}

	// �C�x���g�^�C�}�[���Z�b�g����
	timeBeginPeriod(1); // �^�C�}�̕���\�͂��P�����ɂ���
	g_timerid = timeSetEvent(16, 1, TimerProc, 1, TIME_PERIODIC);

	while(1){	// ���b�Z�[�W����[�v
		if( !GetMessage(&msg, NULL, 0, 0) ){	// ���b�Z�[�W���擾
			break; 
		}else{
			TranslateMessage(&msg); 			// �������b�Z�[�W�ւ̃R���o�[�g�j
			DispatchMessage(&msg); 				// ���b�Z�[�W��WndProc�֑���
		}
	}

	// �Q�[���I���t���O���Z�b�g����
	game_controller->setEndFlag();

	if( g_timerid ) timeKillEvent(g_timerid);	// �}���`���f�B�A�^�C�}�̏I��
	timeEndPeriod(1);							// �^�C�}�̕���\�͂��Ƃɖ߂�

	delete game_controller;
	return (int)msg.wParam;
}

//==============================================================================
//!	@fn		WindowProc
//!	@brief	�E�C���h�E�v���V�[�W��
//!	@param	�E�C���h�E�n���h��
//!	@param	���b�Z�[�W
//!	@param	W�p�����[�^
//!	@param	L�p�����[�^
//!	@retval	�I����
//==============================================================================
LRESULT WINAPI WndProc(	HWND hwnd, 		// �E�B���h�E�n���h��
						UINT message,	// ���b�Z�[�W���ʎq
						WPARAM wParam,	// �t�я��P
						LPARAM lParam)	// �t�я��Q
{
	switch( message ){
	case WM_KEYDOWN:
		switch(wParam){
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

//==============================================================================
//!	@fn		TimerProc
//!	@brief	�P�U�������ɃC�x���g�I�u�W�F�N�g���Z�b�g����
//!	@param	�^�C�}�h�c
//!	@param	���g�p
//!	@param	���g�p
//!	@param	���g�p
//!	@param	���g�p
//!	@retval	�Ȃ�
//==============================================================================
void CALLBACK TimerProc(UINT, UINT, DWORD, DWORD, DWORD)
{
	game_controller->setEvent();
}

//******************************************************************************
//	End of file.
//******************************************************************************
