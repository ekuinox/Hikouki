//*****************************************************************************
//!	@file	CDirectXGraphics.h
//!	@brief	
//!	@note	CDirectXGraphicsクラス
//!	@author
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<d3d9.h>
#include	<d3dx9.h>

//=============================================================================
//!	@class	CDirectXGraphics
//!	@brief	CDirectXGraphicsクラス
//=============================================================================
class CDirectXGraphics{
private:
	LPDIRECT3D9					m_lpd3d;			// DIRECT3D8オブジェクト
	LPDIRECT3DDEVICE9			m_lpd3ddevice;		// DIRECT3DDEVICE8オブジェクト
	D3DPRESENT_PARAMETERS		m_d3dpp;			// プレゼンテーションパラメータ
	D3DDISPLAYMODE				m_disp;				// ディスプレイモード
	int							m_adapter;			// ディスプレイアダプタ番号
	int							m_width;			// バックバッファＸサイズ
	int							m_height;			// バックバッファＹサイズ
public:
	CDirectXGraphics(){								// コンストラクタ
		m_lpd3d = NULL;
		m_lpd3ddevice = NULL;
		m_adapter = 0;
		m_height = 0;
		m_width = 0;
	}

	bool	Init(HWND hwnd,bool fullscreenflag,int width,int height);	// 初期処理
	void	Exit();														// 終了処理
	LPDIRECT3DDEVICE9	GetDXDevice() const {							// デバイスオブジェクトＧＥＴ
		return m_lpd3ddevice;
	}
	D3DPRESENT_PARAMETERS GetDXD3dpp() const{							// プレゼンテーションパラメータＧＥＴ
		return m_d3dpp;
	}
};

//******************************************************************************
//	End of file.
//******************************************************************************