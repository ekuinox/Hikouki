/*-------------------------------
	�f�o�b�O�����N���X
---------------------------------*/

#include <d3dx9.h>

class CDebugFont{
private:
	LPD3DXFONT		m_pFont;
public:
	CDebugFont():m_pFont(NULL){
	}
	virtual ~CDebugFont(){
		if(m_pFont!=NULL){
			m_pFont->Release();
		}
		m_pFont=NULL;
	}

	void CreateFont(const LPDIRECT3DDEVICE9 lpdev){
		D3DXCreateFont(lpdev,						// �f�o�C�X�I�u�W�F�N�g
						0,							// �����̍���
						10,							// �����̕�
						FW_REGULAR,					// �����̑���
						NULL,						// �~�b�v�}�b�v���x���̐�
						FALSE,						// �C�^���b�N�̂��ۂ� true:�C�^���b�N��
						SHIFTJIS_CHARSET,			// �����Z�b�g
						OUT_DEFAULT_PRECIS,			// ��ɂs�q�t�d�s�x�o�d�̃t�H���g���g��
						PROOF_QUALITY,				// �o�͕i��
						FIXED_PITCH | FF_MODERN,	// �s�b�`�ƃt�@�~���[�C���f�b�N�X
						"�l�r�o�S�V�b�N",				// �t�H���g��
						&m_pFont);					// �������ꂽ�t�H���g
	}

	void DrawText(int x,int y,const char* str) const {
		RECT	rect={x,y,0,0};
		// ������̃T�C�Y���v�Z
		m_pFont->DrawText(NULL,str,-1,&rect,DT_CALCRECT ,NULL);
		// ������`��i�ԐF�j
		m_pFont->DrawText(NULL,str,-1,&rect,DT_LEFT | DT_BOTTOM,0xffff0000);
	}
};