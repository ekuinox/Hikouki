//*****************************************************************************
//!	@file	matuutil.cpp
//!	@brief	
//!	@note	���w�n�̃��[�e�B���e�B�֐��Q
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<d3dx9.h>

//==============================================================================
//!	@fn		MakeWorldMatrix
//!	@brief	�p�x�ƕ��s�ړ��ʂ���s������
//!	@param�@�s��i�o�́j�@�@	
//!	@param�@	�p�x�i���́j
//!	@param�@	���s�ړ��ʁi���́j
//!	@retval	�Ȃ�
//==============================================================================
void MakeWorldMatrix(D3DXMATRIX& mat,const D3DXVECTOR3& angle,const D3DXVECTOR3& trans){
	D3DXMATRIX matx, maty, matz, mattrans;;
	D3DXMatrixRotationX(&matx, D3DX_PI*angle.x / 180.0f);
	D3DXMatrixRotationY(&maty, D3DX_PI*angle.y / 180.0f);
	D3DXMatrixRotationZ(&matz, D3DX_PI*angle.z / 180.0f);
	mat = matx*maty*matz;
	D3DXMatrixTranslation(&mattrans, trans.x, trans.y, trans.z);
	
	mat = mat*mattrans;
}

//==============================================================================
//!	@fn		MakeWorldMatrix
//!	@brief	�p�x�ƕ��s�ړ��ʂ���s������(�ώZ�s����v�Z)
//!	@param�@�s��i�o�́j�@�@	
//!	@param�@�ώZ�s��i�o�́j�@�@	
//!	@param�@�p�x�i���́j
//!	@param�@���s�ړ��ʁi���́j
//!	@retval	�Ȃ�
//==============================================================================
void MakeWorldMatrix(D3DXMATRIX& mat, D3DXMATRIX& mattotal,const D3DXVECTOR3& angle, const D3DXVECTOR3& trans){
	D3DXMATRIX matx, maty, matz, mattrans;;

	D3DXMatrixRotationX(&matx, D3DX_PI*angle.x / 180.0f);			// X����]�s��𐶐�
	D3DXMatrixRotationY(&maty, D3DX_PI*angle.y / 180.0f);			// Y����]�s��𐶐�
	D3DXMatrixRotationZ(&matz, D3DX_PI*angle.z / 180.0f);			// Z����]�s��𐶐�
	mat = matx*maty*matz;											// ����

	D3DXMatrixTranslation(&mattrans, trans.x, trans.y, trans.z);	// ���s�ړ��s��𐶐�

	mat = mat*mattrans;
	mattotal = mat*mattotal;
}

//******************************************************************************
//	End of file.
//******************************************************************************
