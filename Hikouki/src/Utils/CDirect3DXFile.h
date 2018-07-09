//*****************************************************************************
//!	@file	CDirect3DXFile.h
//!	@brief	
//!	@note	Xファイルクラス
//!	@author
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include <stdexcept>
//=============================================================================
//!	@class	CDirect3DXFile
//!	@brief	Xファイルクラス
//=============================================================================
class CDirect3DXFile{
private:
	LPD3DXMESH					m_lpmesh;				// メッシュインターフェイス
	D3DMATERIAL9*				m_lpmeshmaterials;		// モデルデータのマテリアル
	LPDIRECT3DTEXTURE9*			m_lpmeshtextures;		// モデルデータのテクスチャ
	DWORD						m_nummaterial;			// マテリアル数
public:
	CDirect3DXFile(){
		m_lpmeshmaterials=NULL;
		m_lpmeshtextures=NULL;
		m_lpmesh=NULL;
	}
	CDirect3DXFile(const char*, LPDIRECT3DDEVICE9);
	virtual ~CDirect3DXFile()
	{
		UnLoadXFile();
	}

	bool LoadXFile(const char* xfilename,	LPDIRECT3DDEVICE9 lpd3ddevice);
	LPD3DXMESH GetMesh() const;
	void UnLoadXFile();
	void Draw(LPDIRECT3DDEVICE9 lpd3ddevice);
	void DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice);
	void DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice);
	LPDIRECT3DTEXTURE9* GetTextures();
};

//******************************************************************************
//	End of file.
//******************************************************************************