//*****************************************************************************
//!	@file	explosion.cpp
//!	@brief	
//!	@note	���������̂��߂̊֐��Q
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	"CDirect3DXFile.h"
#include	"explosion.h"

//-----------------------------------------------------------------------------
//	�O���[�o���ϐ�
//-----------------------------------------------------------------------------
MyVertex	*g_LocalVertex = nullptr;			// ���_���
int			g_NumVertex=0;						// ���_��
MyConnect	*g_Connect = nullptr;				// �������
int			g_NumFace=0;						// �ʐ�
MyTriangle	*g_pTriangle = nullptr;				// �g���C�A���O���̐擪
LPDIRECT3DTEXTURE9	g_pTex1 = nullptr;			// �e�N�X�`��

//==============================================================================
//!	@fn		TriangleTransforms
//!	@brief	�΂�΂�ȂR�p�`���_���i���[�J�����W�j�����[���h���W�n�ɕϊ�
//!	@param�@���[���h�ϊ��s��(����)�@�@	
//!	@retval	�Ȃ�
//==============================================================================
void TriangleTransforms(const D3DXMATRIX& mat)
{
	int				i;
	MyVertex		*TransformWork;	// ���[���h���W�n�ɕϊ����邽�߂̃��[�N
	D3DXVECTOR3		temp;

	int				idx0, idx1, idx2;
	D3DXVECTOR3		normal;
	D3DXVECTOR3		p0p1;
	D3DXVECTOR3		p0p2;

	// �������ꂽ�O�p�`��񂪂���΍폜����
	if (g_pTriangle != nullptr){
		delete[] g_pTriangle;
		g_pTriangle = nullptr;
	}

	TransformWork = new MyVertex[g_NumVertex];		// ���W�ϊ��p���[�N�𒸓_������������

	// ���_�����ׂĎ��o�����[���h���W�ɕϊ�����
	for (i = 0; i<g_NumVertex; i++){
		TransformWork[i] = g_LocalVertex[i];		// ���[�J�����W�����W�ϊ��p���[�N�ɃZ�b�g

		temp.x = g_LocalVertex[i].x;
		temp.y = g_LocalVertex[i].y;
		temp.z = g_LocalVertex[i].z;

		D3DXVec3TransformCoord(&temp, &temp, &mat);	// ���W�ϊ�

		TransformWork[i].x = temp.x;				// ���W�l�̂݃��[���h���W�ŏ�������
		TransformWork[i].y = temp.y;
		TransformWork[i].z = temp.z;
	}

	g_pTriangle = new MyTriangle[g_NumFace];			// �O�p�`�̖ʐ����I�u�W�F�N�g����

	for (i = 0; i<g_NumFace; i++){						// �R�p�`�|���S���������[�v
		idx0 = g_Connect[i].idx[0];
		idx1 = g_Connect[i].idx[1];
		idx2 = g_Connect[i].idx[2];

		g_pTriangle[i].Vertex[0] = TransformWork[idx0];
		g_pTriangle[i].Vertex[0].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// �f�B�t���[�Y�l���Z�b�g����B�i�w�t�@�C���Ɋ܂܂�Ă��Ȃ��ׁj

		g_pTriangle[i].Vertex[1] = TransformWork[idx1];
		g_pTriangle[i].Vertex[1].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// �f�B�t���[�Y�l���Z�b�g����B�i�w�t�@�C���Ɋ܂܂�Ă��Ȃ��ׁj

		g_pTriangle[i].Vertex[2] = TransformWork[idx2];
		g_pTriangle[i].Vertex[2].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// �f�B�t���[�Y�l���Z�b�g����B�i�w�t�@�C���Ɋ܂܂�Ă��Ȃ��ׁj

		p0p1.x = g_pTriangle[i].Vertex[1].x - g_pTriangle[i].Vertex[0].x;
		p0p1.y = g_pTriangle[i].Vertex[1].y - g_pTriangle[i].Vertex[0].y;
		p0p1.z = g_pTriangle[i].Vertex[1].z - g_pTriangle[i].Vertex[0].z;

		p0p2.x = g_pTriangle[i].Vertex[2].x - g_pTriangle[i].Vertex[0].x;
		p0p2.y = g_pTriangle[i].Vertex[2].y - g_pTriangle[i].Vertex[0].y;
		p0p2.z = g_pTriangle[i].Vertex[2].z - g_pTriangle[i].Vertex[0].z;

		// �@�����v�Z
		D3DXVec3Cross(&normal, &p0p1, &p0p2);			// �O�ς��v�Z
		D3DXVec3Normalize(&normal, &normal);			// �P�ʃx�N�g����
		g_pTriangle[i].nx = normal.x;					// �@�����Z�b�g
		g_pTriangle[i].ny = normal.y;
		g_pTriangle[i].nz = normal.z;

		// �d�S�v�Z
		g_pTriangle[i].cx = (g_pTriangle[i].Vertex[0].x + g_pTriangle[i].Vertex[1].x + g_pTriangle[i].Vertex[2].x) / 3;
		g_pTriangle[i].cy = (g_pTriangle[i].Vertex[0].y + g_pTriangle[i].Vertex[1].y + g_pTriangle[i].Vertex[2].y) / 3;
		g_pTriangle[i].cz = (g_pTriangle[i].Vertex[0].z + g_pTriangle[i].Vertex[1].z + g_pTriangle[i].Vertex[2].z) / 3;

		// ���_���W�����_����Ƃ������W�ɕϊ�����
		g_pTriangle[i].Vertex[0].x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[0].y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[0].z -= g_pTriangle[i].cz;

		g_pTriangle[i].Vertex[1].x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[1].y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[1].z -= g_pTriangle[i].cz;

		g_pTriangle[i].Vertex[2].x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[2].y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[2].z -= g_pTriangle[i].cz;
	}

	delete[] TransformWork;
	return;
}

//==============================================================================
//!	@fn		XfileToTriangle
//!	@brief	�w�t�@�C�����΂�΂�ȂR�p�`�v���~�e�B�u�ɂ΂炷
//!	@param�@���b�V���I�u�W�F�N�g�|�C���^(����)�@�@	
//!	@param�@�f�o�C�X�I�u�W�F�N�g(����)�@�@	
//!	@retval	�Ȃ�
//==============================================================================
void XfileToTriangle(LPD3DXMESH lpmesh, LPDIRECT3DDEVICE9 lpdevice)
{
	LPDIRECT3DVERTEXBUFFER9		lpVertexBuffer;	// ���_�o�b�t�@
	LPDIRECT3DINDEXBUFFER9		lpIndexBuffer;	// �C���f�b�N�X�o�b�t�@
	int							length;			// ���_�t�H�[�}�b�g���i�o�C�g�j

	HRESULT			hr;
	LPD3DXMESH		clonemesh;					// ���b�V���C���^�[�t�F�C�X

	BYTE			*data;
	MyVertex		*pVertex;
	WORD			*pIndex;

	// �����p�̃��b�V�����쐬
	hr = lpmesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1), lpdevice, &clonemesh);
	if (FAILED(hr)){
		MessageBox(0, "error ", "error ", MB_OK);
	}

	g_NumVertex = clonemesh->GetNumVertices();		// ���_�����擾

	clonemesh->GetVertexBuffer(&lpVertexBuffer);	// ���_�o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^���Q�b�g
	g_NumFace = clonemesh->GetNumFaces();			// �ʐ����Q�b�g
	clonemesh->GetIndexBuffer(&lpIndexBuffer);		// �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^���Q�b�g

	length = sizeof(MyVertex);

	// ���_�o�b�t�@���b�N
	hr = lpVertexBuffer->Lock(0, length, (void**)&data, D3DLOCK_READONLY);
	if (hr == D3D_OK){
		g_LocalVertex = new MyVertex[g_NumVertex];
		pVertex = (MyVertex*)data;

		// ���_�����R�s�[
		memcpy(g_LocalVertex, pVertex, sizeof(MyVertex)*(g_NumVertex));

		length = sizeof(WORD);
		// �C���f�b�N�X�o�b�t�@���b�N
		hr = lpIndexBuffer->Lock(0, length, (void**)&pIndex, D3DLOCK_READONLY);

		if (hr == D3D_OK){
			g_Connect = new MyConnect[g_NumFace];	// �ʐ����̌��������擾

			for (int i = 0; i<g_NumFace; i++){		// �R�p�`�|���S���������[�v
				g_Connect[i].idx[0] = *pIndex++;
				g_Connect[i].idx[1] = *pIndex++;
				g_Connect[i].idx[2] = *pIndex++;
			}
			lpIndexBuffer->Unlock();				// �C���f�b�N�X�o�b�t�@���
		}
		// ���_�o�b�t�@���
		lpVertexBuffer->Unlock();
	}
	clonemesh->Release();
	return;
}

//==============================================================================
//!	@fn		ExplosionInit
//!	@brief	�����̂��߂̏����������s��
//!	@param�@X�t�@�C���I�u�W�F�N�g�|�C���^(����)�@�@	
//!	@param�@�f�o�C�X�I�u�W�F�N�g(����)�@�@	
//!	@retval	�Ȃ�
//==============================================================================
void ExplosionInit(CDirect3DXFile*	xobj,LPDIRECT3DDEVICE9 lpdevice){
	// X�t�@�C�����o���o���ȂR�p�`�ɕ�������
	XfileToTriangle(xobj->GetMesh(), lpdevice);
	D3DXCreateTextureFromFile(lpdevice, "assets/koa_1.tga", &g_pTex1);
}

//==============================================================================
//!	@fn		ExplosionUpdate
//!	@brief	�����̍X�V����
//!	@param�@�@�@	
//!	@retval	�Ȃ�
//==============================================================================
void ExplosionUpdate(){
	D3DXMATRIX mat1, mat2, mat3;

	for (int i = 0; i < g_NumFace; i++){

		// ��]�p�x����s����쐬����
		D3DXMatrixRotationX(&mat1, g_pTriangle[i].xangle);
		D3DXMatrixRotationY(&mat2, g_pTriangle[i].yangle);
		D3DXMatrixRotationZ(&mat3, g_pTriangle[i].zangle);

		g_pTriangle[i].mat = mat1*mat2*mat3;

		g_pTriangle[i].mat._41 = g_pTriangle[i].cx;
		g_pTriangle[i].mat._42 = g_pTriangle[i].cy;
		g_pTriangle[i].mat._43 = g_pTriangle[i].cz;

		// ��]�p�x�̑�������
		// �������R�[�f�B���O
		// �\�\�\�\�\ start -----------


		// �\�\�\�\�\ end -----------

		// �@�������ւ̈ړ�
		// �������R�[�f�B���O
		// �\�\�\�\�\ start -----------


		// �\�\�\�\�\ end -----------

		for (int j = 0; j<3; j++){
			unsigned char alpha = (unsigned char)((g_pTriangle[i].Vertex[j].col & 0xff000000) >> 24);
			if (alpha > 0){
				g_pTriangle[i].Vertex[j].col -= 0x01000000;	// �A���t�@�l�̕ύX
			}
			else{
				g_pTriangle[i].Vertex[j].col = 0x00ffffff;	// �A���t�@�l��0�ȉ��Ȃ�0��
			}
		}

	}
}

//==============================================================================
//!	@fn		ExplosionDraw
//!	@brief	������`�悷��
//!	@param�@�f�o�C�X�I�u�W�F�N�g(����)�@�@	
//!	@retval	�Ȃ�
//==============================================================================
void ExplosionDraw(LPDIRECT3DDEVICE9 lpdevice){

	lpdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpdevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpdevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	lpdevice->SetRenderState(D3DRS_LIGHTING, true);			//   �����v�Z�I���i���_�̐F���L���ɂȂ�j

	// �e�N�X�`���X�e�[�W�X�e�[�g
	lpdevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpdevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	lpdevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	lpdevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpdevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpdevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	lpdevice->SetTexture(0, g_pTex1);

	for (int i = 0; i<g_NumFace; i++){

		//���_�̌^���Z�b�g
		lpdevice->SetFVF(D3DFVFCUSTOM_VERTEX);
		// ���[���h�ϊ��s����Z�b�g
		lpdevice->SetTransform(D3DTS_WORLD, &g_pTriangle[i].mat);
		//�����_�����O
		lpdevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &g_pTriangle[i].Vertex[0], sizeof(MyVertex));

	}
}


//==============================================================================
//!	@fn		ExplosionExit
//!	@brief	���������̌�n��
//!	@param�@�f�o�C�X�I�u�W�F�N�g(����)�@�@	
//!	@retval	�Ȃ�
//==============================================================================
void ExplosionExit(){

	// �������ꂽ�O�p�`��񂪂���΍폜����
	if (g_pTriangle != nullptr){
		delete[] g_pTriangle;
		g_pTriangle = nullptr;
	}

	if (g_pTex1 != nullptr){
		g_pTex1->Release();
	}
}

//******************************************************************************
//	End of file.
//******************************************************************************
