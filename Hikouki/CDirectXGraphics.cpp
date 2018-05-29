//*****************************************************************************
//!	@file	CDirectXGraphics.cpp
//!	@brief	DiretcXGraphics処理クラス
//!	@note	
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	"CDirectXGraphics.h"

//==============================================================================
//!	@fn		Init
//!	@brief	初期化処理
//!	@param	ウインドウハンドル値
//!	@param	フルスクリーンフラグ（０：ウインドウモード  １：フルスクリーン）
//!	@param	ウインドウサイズ（幅）
//!	@param	ウインドウサイズ（高さ）
//!	@retval	TRUE　成功終了/FALSE　失敗終了
//!	@note	
//==============================================================================
bool CDirectXGraphics::Init(HWND hwnd,bool fullscreenflag,int width,int height)
{
	HRESULT	hr;

	m_lpd3d = Direct3DCreate9(D3D_SDK_VERSION);	// Direct3D9インターフェース取得
	if( m_lpd3d == NULL )	return false;

	// アダプタの現在のディスプレイモードを取得する
	hr = m_lpd3d->GetAdapterDisplayMode(m_adapter,&m_disp);
	if( hr != D3D_OK )	return false;

	memset(&m_d3dpp,0,sizeof(m_d3dpp));			// ゼロクリア
	m_d3dpp.BackBufferFormat = m_disp.Format;	// 現在のビット深度
	m_d3dpp.BackBufferWidth  = width;			// バックバッファの幅をセット
	m_d3dpp.BackBufferHeight = height;			// バックバッファの高さをセット
	m_d3dpp.BackBufferCount = 1;				// バックバッファの数
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// バックバッファをロック可能にする

	if( fullscreenflag )	m_d3dpp.Windowed = FALSE;	// フルスクリーンモード
	else					m_d3dpp.Windowed = TRUE;	// ウインドウモード

	// Ｚバッファの自動作成（ビット深度24）
	m_d3dpp.EnableAutoDepthStencil = 1;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;	// VSYNCを待たない

	// デバイス作成
	hr = m_lpd3d->CreateDevice(	m_adapter,
								D3DDEVTYPE_HAL,
								hwnd,
								D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
								&m_d3dpp,
								&m_lpd3ddevice);
	if( hr != D3D_OK ){
		hr = m_lpd3d->CreateDevice(	m_adapter,
									D3DDEVTYPE_HAL,
									hwnd,
									D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
									&m_d3dpp,
									&m_lpd3ddevice);
		if( hr != D3D_OK){
			hr = m_lpd3d->CreateDevice(	m_adapter,
										D3DDEVTYPE_REF,
										hwnd,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
										&m_d3dpp,
										&m_lpd3ddevice);
			if(hr!=D3D_OK)		return(false);
		}
	}
	m_height = height;
	m_width  = width;
	return(true);
}

//==============================================================================
//!	@fn		Exit
//!	@brief	DirectX Grpaphics の終了処理
//!	@param	
//!	@retval	なし
//!	@note	
//==============================================================================
void CDirectXGraphics::Exit()
{
	if( m_lpd3ddevice != NULL ){
		m_lpd3ddevice->Release();
		m_lpd3ddevice=NULL;
	}
	if( m_lpd3d != NULL ){
		m_lpd3d->Release();
		m_lpd3d=NULL;
	}
	return;
}

void CDirectXGraphics::SetRenderStateArray(std::vector<std::pair<D3DRENDERSTATETYPE, DWORD>> options)
{
	for (const auto& option : options) m_lpd3ddevice->SetRenderState(option.first, option.second);
}

void CDirectXGraphics::Render(std::function<void(void)> render)
{
	m_lpd3ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	m_lpd3ddevice->BeginScene();
	render();
	m_lpd3ddevice->EndScene();
	if (m_lpd3ddevice->Present(NULL, NULL, NULL, NULL) != D3D_OK) { // バックバッファからプライマリバッファへ転送
		m_lpd3ddevice->Reset(&m_d3dpp);
	}
}
//******************************************************************************
//	End of file.
//******************************************************************************
