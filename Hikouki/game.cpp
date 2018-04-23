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

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
CDirectXGraphics	*g_DXGrobj = NULL;		// DirectX Graphics�I�u�W�F�N�g
CDirect3DXFile		*g_DXXFileObj = NULL;	// �w�t�@�C���I�u�W�F�N�g

D3DXMATRIX			g_MatView;			// �J�����s��
D3DXMATRIX			g_MatProjection;	// �v���W�F�N�V�����ϊ��s��
D3DXMATRIX			g_MatWorld;			// ���[���h�ϊ��s��
D3DXMATRIX		    pol_mat; // ��s�@�̍s�񂪂͂���͂�

HANDLE				g_hEventHandle;		// �C�x���g�n���h��
bool				g_EndFlag = false;	// �I���t���O
std::thread			g_gamemainthread;	// �Q�[�����C���X���b�h

using Data = struct _Data {
	struct COR { float x, y, z; } cor;
	struct ROT { float x, y, z; } rot;
	D3DXMATRIX mat;
	CDirect3DXFile *xfile;
	bool explosion_flag;
	Explosion* explosion;
	_Data(CDirect3DXFile* _xfile) : xfile(_xfile) {
		xfile->LoadXFile("assets/f1.x", g_DXGrobj->GetDXDevice());
		explosion = new Explosion(xfile, g_DXGrobj->GetDXDevice());
		explosion_flag = false;
	};
};

std::vector<Data*> data;

D3DXMATRIX			g_MatTotal = {			// �ώZ�s��i�P�ʍs��ŏ������j
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};



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
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height,bool fullscreen)
{
	// ���͏�����
	InitKeyboard(hinst, hwnd);

	bool sts;
	g_DXGrobj = new CDirectXGraphics();	// DirectX Graphics�I�u�W�F�N�g����
	sts = g_DXGrobj->Init(hwnd, fullscreen, width, height);	// DirectX Graphics�I�u�W�F�N�g������

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "DirectX �������G���[", MB_OK);
		return false;
	}

	constexpr int hikouki_count = 9;
	constexpr float margin = 50.0f;
	auto xfile = new CDirect3DXFile(); // �ǂ��������Ȃ̂�

	for (auto i = 0; i < sqrt(hikouki_count); ++i)
	{
		for (auto j = 0; j < sqrt(hikouki_count); ++j)
		{
			data.push_back(new Data(xfile));
			data.back()->cor = { j * margin - margin, i * margin - margin,  0.0f };
			data.back()->rot = { 0.0f, 0.0f, 0.0f };
		}
	}

	// �J�����ϊ��s��쐬
	D3DXMatrixLookAtLH(&g_MatView,
		&D3DXVECTOR3(0.0f, 0.0f, -80.0f),		// ���_
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// �����_
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));		// �����

	// �J�����s����Œ�p�C�v���C���փZ�b�g
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &g_MatView);

	// �v���W�F�N�V�����ϊ��s��쐬
	D3DXMatrixPerspectiveFovLH(&g_MatProjection,
		D3DX_PI / 2,					// ����p
		(float)width / (float)height,	// �A�X�y�N�g��
		1.0f,						// �j�A�v���[��
		1000.0f);					// �t�@�[�v���[��

	// �ˉe�ϊ��s����Œ�p�C�v���C���փZ�b�g
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &g_MatProjection);
	// �y�o�b�t�@�L��
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ���C�g�L��
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// �J�����O������
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// �����Z�b�g
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

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

	if (GetKeyboardPress(DIK_UPARROW)) {
		for (const auto& hikouki : data) hikouki->rot.y--;
	}
	if (GetKeyboardPress(DIK_DOWNARROW)) {
		for (const auto& hikouki : data) hikouki->rot.y++;
	}
	if (GetKeyboardPress(DIK_RIGHTARROW)) {
		for (const auto& hikouki : data) hikouki->rot.x--;
	}
	if (GetKeyboardPress(DIK_LEFTARROW)) {
		for (const auto& hikouki : data) hikouki->rot.x++;
	}

	int keys[] = {
		DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9
	};

	for (auto i = 0; i < 9; i++) {
		if (GetKeyboardTrigger(keys[i]))
		{
			data[i]->explosion_flag = !data[i]->explosion_flag;
			if (data[i]->explosion_flag) {
				data[i]->explosion->TriangleTransforms(data[i]->mat);
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

	for (const auto& hikouki : data)
	{
		D3DXMATRIX mx;

		D3DXMatrixIdentity(&hikouki->mat);

		//	��]����
		D3DXMatrixIdentity(&mx);
		D3DXMatrixRotationX(&mx, D3DXToRadian(hikouki->rot.y));
		D3DXMatrixMultiply(&hikouki->mat, &hikouki->mat, &mx);

		D3DXMatrixIdentity(&mx);
		D3DXMatrixRotationY(&mx, D3DXToRadian(hikouki->rot.x));
		D3DXMatrixMultiply(&hikouki->mat, &hikouki->mat, &mx);

		// �ړ�����
		D3DXMatrixIdentity(&mx);
		D3DXMatrixTranslation(&mx, hikouki->cor.x, hikouki->cor.y, hikouki->cor.z);
		D3DXMatrixMultiply(&hikouki->mat, &hikouki->mat, &mx);

		if (hikouki->explosion_flag) hikouki->explosion->Update();
	}
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameRender(){
	HRESULT  hr;

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
			D3DXMatrixIdentity(&g_MatWorld);
			D3DXMatrixMultiply(&g_MatWorld, &g_MatWorld, &hikouki->mat);
			g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatWorld);	// ���[���h�ϊ��s����Z�b�g
			hikouki->xfile->DrawWithAxis(g_DXGrobj->GetDXDevice());				// �w�t�@�C���`��
		}
	}

	g_DXGrobj->GetDXDevice()->EndScene();	// �`��̏I����҂�

	hr = g_DXGrobj->GetDXDevice()->Present(NULL, NULL, NULL, NULL);	// �o�b�N�o�b�t�@����v���C�}���o�b�t�@�֓]��
	if (hr != D3D_OK){
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
