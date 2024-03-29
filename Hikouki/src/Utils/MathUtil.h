//*****************************************************************************
//!	@file	mathutil.h
//!	@brief	ゲーム処理
//!	@note	
//!	@author
//*****************************************************************************
#pragma once

#include <d3dx9.h>

//-----------------------------------------------------------------------------
// プロトタイプ宣言
//-----------------------------------------------------------------------------
namespace mathutils {
	void makeWorldMatrix(D3DXMATRIX& mat, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans);
	void makeWorldMatrix(D3DXMATRIX& mat, D3DXMATRIX& mattotal, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans);
	const D3DXMATRIX makeWorldMatrixTotal(D3DXMATRIX& mattotal, const D3DXVECTOR3& angle, const D3DXVECTOR3& trans);
}

//******************************************************************************
//	End of file.
//******************************************************************************
