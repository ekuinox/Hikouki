//*****************************************************************************
//!	@file	matuutil.cpp
//!	@brief	
//!	@note	数学系のユーティリティ関数群
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include "MathUtil.h"
//==============================================================================
//!	@fn		makeWorldMatrix
//!	@brief	角度と平行移動量から行列を作る
//!	@param　行列（出力）　　	
//!	@param　	角度（入力）
//!	@param　	平行移動量（入力）
//!	@retval	なし
//==============================================================================
void mathutils::makeWorldMatrix(D3DXMATRIX& mat,const D3DXVECTOR3& angle,const D3DXVECTOR3& trans){
	D3DXMATRIX matx, maty, matz, mattrans;;
	D3DXMatrixRotationX(&matx, D3DX_PI*angle.x / 180.0f);
	D3DXMatrixRotationY(&maty, D3DX_PI*angle.y / 180.0f);
	D3DXMatrixRotationZ(&matz, D3DX_PI*angle.z / 180.0f);
	mat = matx*maty*matz;
	D3DXMatrixTranslation(&mattrans, trans.x, trans.y, trans.z);
	
	mat = mat*mattrans;
}

//==============================================================================
//!	@fn		makeWorldMatrix
//!	@brief	角度と平行移動量から行列を作る(積算行列を計算)
//!	@param　行列（出力）　　	
//!	@param　積算行列（出力）　　	
//!	@param　角度（入力）
//!	@param　平行移動量（入力）
//!	@retval	なし
//==============================================================================
void mathutils::makeWorldMatrix(D3DXMATRIX& mat, D3DXMATRIX& mattotal,const D3DXVECTOR3& angle, const D3DXVECTOR3& trans){
	D3DXMATRIX matx, maty, matz, mattrans;;

	D3DXMatrixRotationX(&matx, D3DX_PI*angle.x / 180.0f);			// X軸回転行列を生成
	D3DXMatrixRotationY(&maty, D3DX_PI*angle.y / 180.0f);			// Y軸回転行列を生成
	D3DXMatrixRotationZ(&matz, D3DX_PI*angle.z / 180.0f);			// Z軸回転行列を生成
	mat = matx*maty*matz;											// 合成

	D3DXMatrixTranslation(&mattrans, trans.x, trans.y, trans.z);	// 平行移動行列を生成

	mat = mat*mattrans;
	mattotal = mat*mattotal;
}

//******************************************************************************
//	End of file.
//******************************************************************************
