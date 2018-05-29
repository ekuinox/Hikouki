/*-------------------------------
	デバッグ文字クラス
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
		D3DXCreateFont(lpdev,						// デバイスオブジェクト
						0,							// 文字の高さ
						10,							// 文字の幅
						FW_REGULAR,					// 文字の太さ
						NULL,						// ミップマップレベルの数
						FALSE,						// イタリック体か否か true:イタリック体
						SHIFTJIS_CHARSET,			// 文字セット
						OUT_DEFAULT_PRECIS,			// 常にＴＲＵＥＴＹＰＥのフォントを使う
						PROOF_QUALITY,				// 出力品質
						FIXED_PITCH | FF_MODERN,	// ピッチとファミリーインデックス
						"ＭＳＰゴシック",				// フォント名
						&m_pFont);					// 生成されたフォント
	}

	void DrawText(int x,int y,const char* str) const {
		RECT	rect={x,y,0,0};
		// 文字列のサイズを計算
		m_pFont->DrawText(NULL,str,-1,&rect,DT_CALCRECT ,NULL);
		// 文字列描画（赤色）
		m_pFont->DrawText(NULL,str,-1,&rect,DT_LEFT | DT_BOTTOM,0xffff0000);
	}
};