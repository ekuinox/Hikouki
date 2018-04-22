//*****************************************************************************
//!	@file	explosion.h
//!	@brief	爆発処理関連
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
// マクロ宣言
//-----------------------------------------------------------------------------
#define  D3DFVFCUSTOM_VERTEX		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//-----------------------------------------------------------------------------
// プロトタイプ宣言
//-----------------------------------------------------------------------------

// 頂点情報
struct	MyVertex{
	float		x, y, z;
	DWORD		col;
	float		tu, tv;
};

// 結線情報
struct MyConnect{
	WORD		idx[3];
};

// ３角形情報
struct	MyTriangle{
	MyVertex		Vertex[3];					// ３角形の頂点
	float			xangle;						// 回転角度Ｘ
	float			yangle;						// 回転角度Ｙ
	float			zangle;						// 回転角度Ｚ
	float			dxa;						// 回転角度増分Ｘ
	float			dya;						// 回転角度増分Ｙ
	float			dza;						// 回転角度増分Ｚ
	float			cx, cy, cz;					// 重心座標
	float			nx, ny, nz;					// 法線ベクトル

	D3DXMATRIX		mat;						// 変換行列
	MyTriangle(){
		xangle = 0.0f;							// 回転角度Ｘ初期化
		yangle = 0.0f;							// 回転角度Ｙ初期化
		zangle = 0.0f;							// 回転角度Ｚ初期化
		dxa = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// 増分初期化Ｘ
		dya = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// 増分初期化Ｙ
		dza = (float)((float)(rand() / (float)RAND_MAX)*0.4);				// 増分初期化Ｚ
		D3DXMatrixIdentity(&mat);
	}
};

class Explosion {
private:
	MyVertex *vertex = nullptr; // 頂点情報
	int	num_vertex = 0; // 頂点数
	MyConnect *connect = nullptr; // 結線情報
	int num_face = 0; // 面数
	MyTriangle	*triangle = nullptr; // トライアングルの先頭
	LPDIRECT3DTEXTURE9 tex = nullptr; // テクスチャ
public:
	Explosion();
	Explosion(CDirect3DXFile *xobj, LPDIRECT3DDEVICE9 device);
	~Explosion();
	void Init(CDirect3DXFile *xobj, LPDIRECT3DDEVICE9 device);
	void Uninit();
	void Update();
	void Draw(LPDIRECT3DDEVICE9 device);
	void TriangleTransforms(const D3DXMATRIX& mat);
	void XfileToTriangle(LPD3DXMESH mesh, LPDIRECT3DDEVICE9 device);
};
//******************************************************************************
//	End of file.
//******************************************************************************
