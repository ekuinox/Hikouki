//*****************************************************************************
//!	@file	explosion.h
//!	@brief	”š”­ˆ—ŠÖ˜A
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
// ƒ}ƒNƒéŒ¾
//-----------------------------------------------------------------------------
#define  D3DFVFCUSTOM_VERTEX		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//-----------------------------------------------------------------------------
// ƒvƒƒgƒ^ƒCƒvéŒ¾
//-----------------------------------------------------------------------------

// ’¸“_î•ñ
struct	MyVertex{
	float		x, y, z;
	DWORD		col;
	float		tu, tv;
};

// Œ‹üî•ñ
struct MyConnect{
	WORD		idx[3];
};

// ‚RŠpŒ`î•ñ
struct	MyTriangle{
	MyVertex		Vertex[3];					// ‚RŠpŒ`‚Ì’¸“_
	float			xangle;						// ‰ñ“]Šp“x‚w
	float			yangle;						// ‰ñ“]Šp“x‚x
	float			zangle;						// ‰ñ“]Šp“x‚y
	float			dxa;						// ‰ñ“]Šp“x‘•ª‚w
	float			dya;						// ‰ñ“]Šp“x‘•ª‚x
	float			dza;						// ‰ñ“]Šp“x‘•ª‚y
	float			cx, cy, cz;					// dSÀ•W
	float			nx, ny, nz;					// –@üƒxƒNƒgƒ‹

	D3DXMATRIX		mat;						// •ÏŠ·s—ñ
	MyTriangle(){
		xangle = 0.0f;							// ‰ñ“]Šp“x‚w‰Šú‰»
		yangle = 0.0f;							// ‰ñ“]Šp“x‚x‰Šú‰»
		zangle = 0.0f;							// ‰ñ“]Šp“x‚y‰Šú‰»
		dxa = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// ‘•ª‰Šú‰»‚w
		dya = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// ‘•ª‰Šú‰»‚x
		dza = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// ‘•ª‰Šú‰»‚y
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
