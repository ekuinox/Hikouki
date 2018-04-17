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
#include	<d3dx9.h>
#include	"CDirect3DXFile.h"

//-----------------------------------------------------------------------------
// �}�N���錾
//-----------------------------------------------------------------------------
#define  D3DFVFCUSTOM_VERTEX		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

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

void XfileToTriangle(LPD3DXMESH, LPDIRECT3DDEVICE9);
void TriangleTransforms(const D3DXMATRIX&);
void ExplosionDraw(LPDIRECT3DDEVICE9);
void ExplosionInit(CDirect3DXFile*, LPDIRECT3DDEVICE9);
void ExplosionUpdate();
//******************************************************************************
//	End of file.
//******************************************************************************
