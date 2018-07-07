//*****************************************************************************
//!	@file	explosion.h
//!	@brief	���������֘A
//!	@note	
//!	@author
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <d3dx9.h>
#include <chrono>
#include "../Utils/CDirect3DXFile.h"

//-----------------------------------------------------------------------------
// �}�N���錾
//-----------------------------------------------------------------------------
#define  D3DFVFCUSTOM_VERTEX		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define GRAVITY (9.8f)

//-----------------------------------------------------------------------------
// �v���g�^�C�v�錾
//-----------------------------------------------------------------------------

// ���_���
struct	MyVertex{
	float		x, y, z;
	DWORD		col;
	float		tu, tv;
};

// �������
struct MyConnect{
	WORD		idx[3];
};

using time_point = std::chrono::system_clock::time_point;


// �R�p�`���
struct	MyTriangle{
	MyVertex		Vertex[3];					// �R�p�`�̒��_
	float			xangle;						// ��]�p�x�w
	float			yangle;						// ��]�p�x�x
	float			zangle;						// ��]�p�x�y
	float			dxa;						// ��]�p�x�����w
	float			dya;						// ��]�p�x�����x
	float			dza;						// ��]�p�x�����y
	float			cx, cy, cz;					// �d�S���W
	float			nx, ny, nz;					// �@���x�N�g��

	D3DXMATRIX		mat;						// �ϊ��s��
	MyTriangle(){
		xangle = 0.0f;							// ��]�p�x�w������
		yangle = 0.0f;							// ��]�p�x�x������
		zangle = 0.0f;							// ��]�p�x�y������
		dxa = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// �����������w
		dya = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// �����������x
		dza = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// �����������y
		D3DXMatrixIdentity(&mat);
	}
};

class Explosion {
private:
	MyVertex *vertex = nullptr; // ���_���
	int	num_vertex = 0; // ���_��
	MyConnect *connect = nullptr; // �������
	int num_face = 0; // �ʐ�
	MyTriangle	*triangle = nullptr; // �g���C�A���O���̐擪
	LPDIRECT3DTEXTURE9 tex = nullptr; // �e�N�X�`��
	time_point last_update;
public:
	Explosion();
	Explosion(CDirect3DXFile *xobj, LPDIRECT3DDEVICE9 device);
	~Explosion();
	void Init(CDirect3DXFile *xobj, LPDIRECT3DDEVICE9 device);
	void Uninit();
	void Start();
	void Update();
	void Draw(const LPDIRECT3DDEVICE9& device);
	void TriangleTransforms(const D3DXMATRIX& mat);
	void XfileToTriangle(LPD3DXMESH mesh, LPDIRECT3DDEVICE9 device);
};
//******************************************************************************
//	End of file.
//******************************************************************************
