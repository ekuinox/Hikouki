//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	ゲーム処理
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <thread>
#include "CDirectXGraphics.h"
#include "CDirect3DXFile.h"
#include "game.h"
#include "input.h"
#include "explosion.h"
#include "mathutil.h"
#include <vector>

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
CDirectXGraphics	*g_DXGrobj = NULL;		// DirectX Graphicsオブジェクト
CDirect3DXFile		*g_DXXFileObj = NULL;	// Ｘファイルオブジェクト

D3DXMATRIX			g_MatView;			// カメラ行列
D3DXMATRIX			g_MatProjection;	// プロジェクション変換行列
D3DXMATRIX			g_MatWorld;			// ワールド変換行列
D3DXMATRIX		    pol_mat; // 飛行機の行列がはいるはず

HANDLE				g_hEventHandle;		// イベントハンドル
bool				g_EndFlag = false;	// 終了フラグ
std::thread			g_gamemainthread;	// ゲームメインスレッド

using Data = struct _Data {
	struct COR { float x, y, z; } cor;
	struct ROT { float x, y, z; } rot;
	D3DXMATRIX mat;
	CDirect3DXFile *xfile;
	bool explosion_flag;
	Explosion* explosion;
	_Data(CDirect3DXFile* _xfile) : xfile(_xfile) {
		xfile->LoadXFile("assets/f1.x", g_DXGrobj->GetDXDevice());
		explosion = new Explosion(xfile, g_DXGrobj->GetDXDevice());
		explosion_flag = false;
	};
};

std::vector<Data*> data;

D3DXMATRIX			g_MatTotal = {			// 積算行列（単位行列で初期化）
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};



//==============================================================================
//!	@fn		GameInit
//!	@brief	ゲーム初期処理
//!	@param	インスタンス値
//!	@param	ウインドウハンドル値
//!	@param	ウインドウサイズ幅（補正済みの値）
//!	@param	ウインドウサイズ高さ（補正済みの値）
//!	@param　	フルスクリーンフラグ　true ; フルスクリーン　false : ウインドウ
//!	@retval	true 成功　false 失敗
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height,bool fullscreen)
{
	// 入力初期化
	InitKeyboard(hinst, hwnd);

	bool sts;
	g_DXGrobj = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成
	sts = g_DXGrobj->Init(hwnd, fullscreen, width, height);	// DirectX Graphicsオブジェクト初期化

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "DirectX 初期化エラー", MB_OK);
		return false;
	}

	constexpr int hikouki_count = 9;
	constexpr float margin = 50.0f;
	auto xfile = new CDirect3DXFile(); // どうせ同じなので

	for (auto i = 0; i < sqrt(hikouki_count); ++i)
	{
		for (auto j = 0; j < sqrt(hikouki_count); ++j)
		{
			data.push_back(new Data(xfile));
			data.back()->cor = { j * margin - margin, i * margin - margin,  0.0f };
			data.back()->rot = { 0.0f, 0.0f, 0.0f };
		}
	}

	// カメラ変換行列作成
	D3DXMatrixLookAtLH(&g_MatView,
		&D3DXVECTOR3(0.0f, 0.0f, -80.0f),		// 視点
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 注視点
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));		// 上向き

	// カメラ行列を固定パイプラインへセット
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &g_MatView);

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(&g_MatProjection,
		D3DX_PI / 2,					// 視野角
		(float)width / (float)height,	// アスペクト比
		1.0f,						// ニアプレーン
		1000.0f);					// ファープレーン

	// 射影変換行列を固定パイプラインへセット
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &g_MatProjection);
	// Ｚバッファ有効
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ライト有効
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// カリング無効化
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// 環境光セット
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	// イベントハンドル生成
	g_hEventHandle = CreateEvent(NULL, false, false, NULL);
	if (g_hEventHandle == NULL){
		MessageBox(hwnd, "ERROR!!", "CreateEvent エラー", MB_OK);
		return false;
	}
	// スレッド生成(ゲームメイン)
	g_gamemainthread = std::thread(GameMain);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	ゲーム入力検知処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameInput(){
	UpdateInput();

	if (GetKeyboardPress(DIK_UPARROW)) {
		for (const auto& hikouki : data) hikouki->rot.y--;
	}
	if (GetKeyboardPress(DIK_DOWNARROW)) {
		for (const auto& hikouki : data) hikouki->rot.y++;
	}
	if (GetKeyboardPress(DIK_RIGHTARROW)) {
		for (const auto& hikouki : data) hikouki->rot.x--;
	}
	if (GetKeyboardPress(DIK_LEFTARROW)) {
		for (const auto& hikouki : data) hikouki->rot.x++;
	}

	int keys[] = {
		DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9
	};

	for (auto i = 0; i < 9; i++) {
		if (GetKeyboardTrigger(keys[i]))
		{
			data[i]->explosion_flag = !data[i]->explosion_flag;
			if (data[i]->explosion_flag) {
				data[i]->explosion->TriangleTransforms(data[i]->mat);
			}
		}
	}
}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameUpdate(){

	for (const auto& hikouki : data)
	{
		D3DXMATRIX mx;

		D3DXMatrixIdentity(&hikouki->mat);

		//	回転が先
		D3DXMatrixIdentity(&mx);
		D3DXMatrixRotationX(&mx, D3DXToRadian(hikouki->rot.y));
		D3DXMatrixMultiply(&hikouki->mat, &hikouki->mat, &mx);

		D3DXMatrixIdentity(&mx);
		D3DXMatrixRotationY(&mx, D3DXToRadian(hikouki->rot.x));
		D3DXMatrixMultiply(&hikouki->mat, &hikouki->mat, &mx);

		// 移動が後
		D3DXMatrixIdentity(&mx);
		D3DXMatrixTranslation(&mx, hikouki->cor.x, hikouki->cor.y, hikouki->cor.z);
		D3DXMatrixMultiply(&hikouki->mat, &hikouki->mat, &mx);

		if (hikouki->explosion_flag) hikouki->explosion->Update();
	}
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameRender(){
	HRESULT  hr;

	// ターゲットバッファのクリア、Ｚバッファのクリア
	g_DXGrobj->GetDXDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// 描画の開始をＤＩＲＥＣＴＸに通知
	g_DXGrobj->GetDXDevice()->BeginScene();

	// ワールド変換行列の初期化
	for (const auto& hikouki : data)
	{
		if (hikouki->explosion_flag) {
			hikouki->explosion->Draw(g_DXGrobj->GetDXDevice());
		}
		else {
			D3DXMatrixIdentity(&g_MatWorld);
			D3DXMatrixMultiply(&g_MatWorld, &g_MatWorld, &hikouki->mat);
			g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatWorld);	// ワールド変換行列をセット
			hikouki->xfile->DrawWithAxis(g_DXGrobj->GetDXDevice());				// Ｘファイル描画
		}
	}

	g_DXGrobj->GetDXDevice()->EndScene();	// 描画の終了を待つ

	hr = g_DXGrobj->GetDXDevice()->Present(NULL, NULL, NULL, NULL);	// バックバッファからプライマリバッファへ転送
	if (hr != D3D_OK){
		g_DXGrobj->GetDXDevice()->Reset(&g_DXGrobj->GetDXD3dpp());
	}
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	ゲームループ処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameMain()
{
	while (1){
		DWORD sts = WaitForSingleObject(g_hEventHandle, 1000);	// イベントフラグがセットされるのを待つ（1000msでタイムアウト）
		if (sts == WAIT_FAILED){
			break;
		}
		else if (sts == WAIT_TIMEOUT){
			if (g_EndFlag){
				break;
			}
			continue;
		}

		GameInput();					// 入力
		GameUpdate();					// 更新
		GameRender();					// 描画
	}
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	ゲーム終了処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameExit()
{

	g_gamemainthread.join();					// ゲームメインスレッドの終了を待つ

	CloseHandle(g_hEventHandle);				// イベントハンドルクローズ

	if (g_DXXFileObj != NULL){
		delete g_DXXFileObj;					//	Xファイルオブジェクトの解放
	}

	if (g_DXGrobj != NULL){
		g_DXGrobj->Exit();						// DirectXのグラフィックオブジェクトの終了処理
	}

	if (g_DXGrobj != NULL){						// DirectXのグラフィックオブジェクトの解放
		delete g_DXGrobj;
		g_DXGrobj = NULL;
	}
}

//==============================================================================
//!	@fn		GameSetEvent
//!	@brief	ゲームループ内のイベントオブジェクトをセットする
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameSetEvent(){
	if (!g_EndFlag){
		SetEvent(g_hEventHandle);	// 終了フラグがセットされていなければイベントをセットする
	}
}

//==============================================================================
//!	@fn		GameSetEndFlag
//!	@brief	ゲームループの終了フラグをセットする
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameSetEndFlag(){
	g_EndFlag = true;				// 終了フラグをオンにする
}

//******************************************************************************
//	End of file.
//******************************************************************************
