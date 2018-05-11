//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	�Q�[������
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <thread>
#include "CDirectXGraphics.h"
#include "CDirect3DXFile.h"
#include "game.h"
#include "input.h"
#include "explosion.h"
#include "mathutil.h"
#include <vector>
#include <chrono>

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
CDirectXGraphics *g_DXGrobj = NULL; // DirectX Graphics�I�u�W�F�N�g
CDirect3DXFile *g_DXXFileObj = NULL; // �w�t�@�C���I�u�W�F�N�g

HANDLE g_hEventHandle; // �C�x���g�n���h��
bool g_EndFlag = false;	// �I���t���O
std::thread g_gamemainthread; // �Q�[�����C���X���b�h

using Data = struct _Data {
	D3DXVECTOR3 cor;
	D3DXVECTOR3 rot;
	D3DXMATRIX mat;
	CDirect3DXFile *xfile;
	bool explosion_flag;
	Explosion* explosion;
	_Data(CDirect3DXFile* _xfile) : xfile(_xfile) {
		explosion = new Explosion(xfile, g_DXGrobj->GetDXDevice());
		explosion_flag = false;
		mat = { // �ώZ�s��i�P�ʍs��ŏ������j
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	};
};

using Camera = struct _Camera {
	D3DXVECTOR3 looking_at; // ���_
	D3DXVECTOR3 looking_for; // �����_
	D3DXVECTOR3 up;
	_Camera() :
		looking_at(D3DXVECTOR3(0.0, 0.0f, -80.0f)),
		looking_for(D3DXVECTOR3(0.0, 0.0f, 0.0f)),
		up(D3DXVECTOR3(0.0, 1.0f, 0.0f))
	{};
};

std::vector<Data*> data;
Data* skydome;
Camera *camera;
bool tps = false;

int width;
int height;

int now_controll = 4;


//==============================================================================
//!	@fn		GameInit
//!	@brief	�Q�[����������
//!	@param	�C���X�^���X�l
//!	@param	�E�C���h�E�n���h���l
//!	@param	�E�C���h�E�T�C�Y���i�␳�ς݂̒l�j
//!	@param	�E�C���h�E�T�C�Y�����i�␳�ς݂̒l�j
//!	@param�@	�t���X�N���[���t���O�@true ; �t���X�N���[���@false : �E�C���h�E
//!	@retval	true �����@false ���s
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int _width, int _height,bool fullscreen)
{
	width = _width;
	height = _height;

	// ���͏�����
	InitKeyboard(hinst, hwnd);

	g_DXGrobj = new CDirectXGraphics();	// DirectX Graphics�I�u�W�F�N�g����

	if (!g_DXGrobj->Init(hwnd, fullscreen, width, height)){	// DirectX Graphics�I�u�W�F�N�g������
		MessageBox(hwnd, "ERROR!!", "DirectX �������G���[", MB_OK);
		return false;
	}

	constexpr int hikouki_count = 9;
	constexpr float margin = 50.0f;
	std::vector<std::pair<CDirect3DXFile*, const char*>> xfiles = {
		{ new CDirect3DXFile(), "assets/f1.x" }, // ��s�@
		{ new CDirect3DXFile(), "assets/skydome.x" } // �X�J�C�h�[��
	};

	for (const auto& xfile : xfiles) xfile.first->LoadXFile(xfile.second, g_DXGrobj->GetDXDevice());

	skydome = new Data(xfiles[1].first);
	skydome->cor = { 0, 0, 0 };
	skydome->rot = { 0, 0, 0 };
	camera = new Camera();

	for (auto i = 0; i < sqrt(hikouki_count); ++i)
	{
		for (auto j = 0; j < sqrt(hikouki_count); ++j)
		{
			D3DXMATRIX mx;
			data.push_back(new Data(xfiles[0].first));
			data.back()->cor = { 0.0f, 0.0f, 1.0f };
			data.back()->rot = { rand() % 2 - 1.0f, rand() % 2 - 1.0f, rand() % 2 - 1.0f };
			MakeWorldMatrix(mx, data.back()->mat, { 0.0f, 0.0f, 0.0f }, { j * margin - margin, i * margin - margin,  0.0f });
		}
	}

	// �C�x���g�n���h������
	g_hEventHandle = CreateEvent(NULL, false, false, NULL);
	if (g_hEventHandle == NULL){
		MessageBox(hwnd, "ERROR!!", "CreateEvent �G���[", MB_OK);
		return false;
	}
	// �X���b�h����(�Q�[�����C��)
	g_gamemainthread = std::thread(GameMain);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	�Q�[�����͌��m����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameInput(){
	UpdateInput();

	if (GetKeyboardTrigger(DIK_ADD) && now_controll + 1 < data.size()) now_controll++;
	if (GetKeyboardTrigger(DIK_SUBTRACT) && now_controll > 0) now_controll--;
	
	if (GetKeyboardTrigger(DIK_V)) tps = !tps;

	int keys[] = {
		DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9
	};

	for (auto i = 0; i < 9; i++) {
		if (GetKeyboardTrigger(keys[i]))
		{
			data[i]->explosion_flag = !data[i]->explosion_flag;
			if (data[i]->explosion_flag) {
				data[i]->explosion->TriangleTransforms(data[i]->mat);
				data[i]->explosion->Start();
			}
		}
	}
}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameUpdate(){

	static auto last_update = std::chrono::system_clock::now();

	for (const auto& hikouki : data)
	{
		D3DXMATRIX mx;

		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_update).count() > 1)
		{
			hikouki->rot.x = rand() % 2 - 1;
			hikouki->rot.y = rand() % 2 - 1;
		}

		MakeWorldMatrix(mx, hikouki->mat, hikouki->rot, hikouki->cor);

		if (hikouki->explosion_flag) hikouki->explosion->Update();
	}

	if (tps) // tps
	{
		camera->looking_for = D3DXVECTOR3(data[now_controll]->mat._41, data[now_controll]->mat._42, data[now_controll]->mat._43);
		camera->looking_at = camera->looking_for - 10 * D3DXVECTOR3(data[now_controll]->mat._31, data[now_controll]->mat._32, data[now_controll]->mat._33);
		camera->up = D3DXVECTOR3(data[now_controll]->mat._21, data[now_controll]->mat._22, data[now_controll]->mat._23);
	}
	else // fps
	{
		camera->looking_at = 2 * D3DXVECTOR3(data[now_controll]->mat._41, data[now_controll]->mat._42, data[now_controll]->mat._43); // ���炳�Ȃ��Ɩ{�̂Ɣ�����܂��̂�
		camera->looking_for = camera->looking_at + 10 * D3DXVECTOR3(data[now_controll]->mat._31, data[now_controll]->mat._32, data[now_controll]->mat._33);
		camera->up = D3DXVECTOR3(data[now_controll]->mat._21, data[now_controll]->mat._22, data[now_controll]->mat._23);
	}
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameRender(){

	D3DXMATRIX view, proj, world;

	D3DXMatrixLookAtLH(&view, &camera->looking_at, &camera->looking_for, &camera->up);
												// �J�����s����Œ�p�C�v���C���փZ�b�g
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &view);

	// �v���W�F�N�V�����ϊ��s��쐬
	D3DXMatrixPerspectiveFovLH(&proj,
		D3DX_PI / 2,					// ����p
		(float)width / (float)height,	// �A�X�y�N�g��
		1.0f,						// �j�A�v���[��
		5000.0f);					// �t�@�[�v���[��

									// �ˉe�ϊ��s����Œ�p�C�v���C���փZ�b�g
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &proj);
	// �y�o�b�t�@�L��
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ���C�g�L��
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// �J�����O������
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// �����Z�b�g
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	// �^�[�Q�b�g�o�b�t�@�̃N���A�A�y�o�b�t�@�̃N���A
	g_DXGrobj->GetDXDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// �`��̊J�n���c�h�q�d�b�s�w�ɒʒm
	g_DXGrobj->GetDXDevice()->BeginScene();

	// ���[���h�ϊ��s��̏�����
	for (const auto& hikouki : data)
	{
		if (hikouki->explosion_flag) {
			hikouki->explosion->Draw(g_DXGrobj->GetDXDevice());
		}
		else {
			g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &hikouki->mat);	// ���[���h�ϊ��s����Z�b�g
			hikouki->xfile->Draw(g_DXGrobj->GetDXDevice());				// �w�t�@�C���`��
		}
	}

	D3DXMatrixIdentity(&world);
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &world);	// ���[���h�ϊ��s����Z�b�g
	skydome->xfile->Draw(g_DXGrobj->GetDXDevice());

	g_DXGrobj->GetDXDevice()->EndScene();	// �`��̏I����҂�

	if (g_DXGrobj->GetDXDevice()->Present(NULL, NULL, NULL, NULL) != D3D_OK){ // �o�b�N�o�b�t�@����v���C�}���o�b�t�@�֓]��
		g_DXGrobj->GetDXDevice()->Reset(&g_DXGrobj->GetDXD3dpp());
	}
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	�Q�[�����[�v����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameMain()
{
	while (1){
		DWORD sts = WaitForSingleObject(g_hEventHandle, 1000);	// �C�x���g�t���O���Z�b�g�����̂�҂i1000ms�Ń^�C���A�E�g�j
		if (sts == WAIT_FAILED){
			break;
		}
		else if (sts == WAIT_TIMEOUT){
			if (g_EndFlag){
				break;
			}
			continue;
		}

		GameInput();					// ����
		GameUpdate();					// �X�V
		GameRender();					// �`��
	}
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	�Q�[���I������
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameExit()
{
	g_gamemainthread.join();					// �Q�[�����C���X���b�h�̏I����҂�

	CloseHandle(g_hEventHandle);				// �C�x���g�n���h���N���[�Y

	if (g_DXXFileObj != NULL){
		delete g_DXXFileObj;					//	X�t�@�C���I�u�W�F�N�g�̉��
	}

	if (g_DXGrobj != NULL){
		g_DXGrobj->Exit();						// DirectX�̃O���t�B�b�N�I�u�W�F�N�g�̏I������
	}

	if (g_DXGrobj != NULL){						// DirectX�̃O���t�B�b�N�I�u�W�F�N�g�̉��
		delete g_DXGrobj;
		g_DXGrobj = NULL;
	}
}

//==============================================================================
//!	@fn		GameSetEvent
//!	@brief	�Q�[�����[�v���̃C�x���g�I�u�W�F�N�g���Z�b�g����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameSetEvent(){
	if (!g_EndFlag){
		SetEvent(g_hEventHandle);	// �I���t���O���Z�b�g����Ă��Ȃ���΃C�x���g���Z�b�g����
	}
}

//==============================================================================
//!	@fn		GameSetEndFlag
//!	@brief	�Q�[�����[�v�̏I���t���O���Z�b�g����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameSetEndFlag(){
	g_EndFlag = true;				// �I���t���O���I���ɂ���
}

//******************************************************************************
//	End of file.
//******************************************************************************
