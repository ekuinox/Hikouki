//*****************************************************************************
//!	@file	CDirect3DXFile.cpp
//!	@brief	X�t�@�C���ǂݍ��݁A�`��N���X
//!	@note	
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include "CDirect3DXFile.h"

CDirect3DXFile::CDirect3DXFile(const char* xfilename, LPDIRECT3DDEVICE9 lpd3ddevice)
	: CDirect3DXFile()
{
	LoadXFile(xfilename, lpd3ddevice);
}

//==============================================================================
//!	@fn		LoadXFile
//!	@brief	X�t�@�C���ǂݍ���
//!	@param	X�t�@�C����
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	TRUE�@�����I��/FALSE�@���s�I��
//!	@note	
//==============================================================================
void CDirect3DXFile::LoadXFile( const char *xfilename, LPDIRECT3DDEVICE9 lpd3ddevice )
{
	LPD3DXBUFFER pd3dxmtrlbuffer;
	LPD3DXBUFFER pd3dxmtrlbuffer2;

	if (SUCCEEDED(D3DXLoadMeshFromX(xfilename, D3DXMESH_SYSTEMMEM, lpd3ddevice, NULL, &pd3dxmtrlbuffer, &pd3dxmtrlbuffer2, &m_nummaterial, &m_lpmesh)))
	{
		D3DXMATERIAL	*d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();	// �}�e���A���̃A�h���X���擾
		m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];		// �}�e���A���R���e�i�̍쐬
		m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];	// �e�N�X�`���R���e�i�̐���

		for (unsigned int i = 0; i < m_nummaterial; ++i)
		{
			m_lpmeshmaterials[i] = d3dxmaterials[i].MatD3D;
			m_lpmeshmaterials[i].Emissive = m_lpmeshmaterials[i].Specular = m_lpmeshmaterials[i].Ambient = m_lpmeshmaterials[i].Diffuse;

			if (FAILED(D3DXCreateTextureFromFile(lpd3ddevice, d3dxmaterials[i].pTextureFilename, &m_lpmeshtextures[i])))
			{
				m_lpmeshtextures[i] = NULL;
			}
		}
		pd3dxmtrlbuffer->Release();		// �}�e���A���o�b�t�@�̃����[�X
		pd3dxmtrlbuffer2->Release();	// �}�e���A���o�b�t�@�̃����[�X
	}
	else
	{
		throw std::runtime_error("XFile�̓ǂݍ��݂Ɏ��s���܂����D");
	}
}

//==============================================================================
//!	@fn		UnLoadXFile
//!	@brief	X�t�@�C���I�u�W�F�N�g�A�����[�h
//!	@param	
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::UnLoadXFile()
{
	if (m_lpmeshmaterials!=NULL)	// �}�e���A���I�u�W�F�N�g�̉��
	{
		delete [] m_lpmeshmaterials;
		m_lpmeshmaterials=NULL;
	}

	if (m_lpmeshtextures!=NULL)		// �e�N�X�`���I�u�W�F�N�g�̉��
	{
		for (unsigned int i = 0 ; i < m_nummaterial; ++i)
		{
			if (m_lpmeshtextures[i]!=NULL) m_lpmeshtextures[i]->Release();
		}
		delete [] m_lpmeshtextures;
		m_lpmeshtextures = NULL;
	}

	if (m_lpmesh != NULL)		// ���b�V�����
	{
		m_lpmesh->Release();
		m_lpmesh = NULL;
	}
}

//==============================================================================
//!	@fn		Draw
//!	@brief	X�t�@�C���`��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::Draw(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	for (unsigned int i = 0 ; i < m_nummaterial; ++i)
	{
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// �}�e���A���̃Z�b�g
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// �e�N�X�`���̃Z�b�g
		m_lpmesh->DrawSubset(i);							// �T�u�Z�b�g�̕`��
	}
}

//==============================================================================
//!	@fn		DrawWithAxis
//!	@brief	X�t�@�C���`��(3����`�悷��)
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	DrawAxis(lpd3ddevice);	// �R���̕`��

	for (unsigned int i = 0; i < m_nummaterial; ++i)	// �T�u�Z�b�g�̕`��
	{
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// �}�e���A���̃Z�b�g
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// �e�N�X�`���̃Z�b�g
		m_lpmesh->DrawSubset(i);							// �T�u�Z�b�g�̕`��
	}
}

//==============================================================================
//!	@fn		DrawAxis
//!	@brief	3����`�悷��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	struct VERTEXDATA{
		float		x,y,z;
		D3DCOLOR	color;
	};

	// �R���̒��_�f�[�^
	struct VERTEXDATA linevertices[6] = {
		-400,    0,    0,  D3DCOLOR_XRGB(255, 0, 0),	// �w���n�_ ��
		 400,    0,    0,  D3DCOLOR_XRGB(255, 0, 0),	// �w���I�_ ��
		   0, -400,    0,  D3DCOLOR_XRGB(0, 255, 0),	// �x���n�_ ��
		   0,  400,    0,  D3DCOLOR_XRGB(0, 255, 0),	// �x���I�_ ��
		   0,    0, -400,  D3DCOLOR_XRGB(0, 0, 255),	// �y���n�_ ��
		   0,    0,  400,  D3DCOLOR_XRGB(0, 0, 255)		// �y���I�_ ��
	};

	lpd3ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);	// ���_�t�H�[�}�b�g���Z�b�g
	lpd3ddevice->SetRenderState(D3DRS_LIGHTING,false);	//   �����v�Z�I�t�i�����v�Z��؂��DIFFUSE�F�����̂܂܂ł�j

	lpd3ddevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, linevertices, sizeof(VERTEXDATA));	// �R����`��
	lpd3ddevice->SetRenderState(D3DRS_LIGHTING,true);	//   �����v�ZON
}

LPDIRECT3DTEXTURE9 * CDirect3DXFile::GetTextures()
{
	return m_lpmeshtextures;
}

//==============================================================================
//!	@fn		GetMesh
//!	@brief	���b�V���I�u�W�F�N�g�|�C���^��߂�
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
LPD3DXMESH CDirect3DXFile::GetMesh() const{
	return m_lpmesh;
}

//******************************************************************************
//	End of file.
//******************************************************************************