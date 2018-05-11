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
#include <chrono>

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
CDirectXGraphics *g_DXGrobj = NULL; // DirectX Graphicsオブジェクト
CDirect3DXFile *g_DXXFileObj = NULL; // Ｘファイルオブジェクト

HANDLE g_hEventHandle; // イベントハンドル
bool g_EndFlag = false;	// 終了フラグ
std::thread g_gamemainthread; // ゲームメインスレッド

using Data = struct _Data {
	D3DXVECTOR3 cor;
	D3DXVECTOR3 rot;
	D3DXMATRIX mat;
	CDirect3DXFile *xfile;
	bool explosion_flag;
	Explosion* explosion;
	_Data(CDirect3DXFile* _xfile) : xfile(_xfile) {
		explosion = new Explosion(xfile, g_DXGrobj->GetDXDevice());
		explosion_flag = false;
		mat = { // 積算行列（単位行列で初期化）
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	};
};

using Camera = struct _Camera {
	D3DXVECTOR3 looking_at; // 視点
	D3DXVECTOR3 looking_for; // 注視点
	D3DXVECTOR3 up;
	_Camera() :
		looking_at(D3DXVECTOR3(0.0, 0.0f, -80.0f)),
		looking_for(D3DXVECTOR3(0.0, 0.0f, 0.0f)),
		up(D3DXVECTOR3(0.0, 1.0f, 0.0f))
	{};
};

std::vector<Data*> data;
Data* skydome;
Camera *camera;
bool tps = false;

int width;
int height;

int now_controll = 4;


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
bool GameInit(HINSTANCE hinst, HWND hwnd, int _width, int _height,bool fullscreen)
{
	width = _width;
	height = _height;

	// 入力初期化
	InitKeyboard(hinst, hwnd);

	g_DXGrobj = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成

	if (!g_DXGrobj->Init(hwnd, fullscreen, width, height)){	// DirectX Graphicsオブジェクト初期化
		MessageBox(hwnd, "ERROR!!", "DirectX 初期化エラー", MB_OK);
		return false;
	}

	constexpr int hikouki_count = 9;
	constexpr float margin = 50.0f;
	std::vector<std::pair<CDirect3DXFile*, const char*>> xfiles = {
		{ new CDirect3DXFile(), "assets/f1.x" }, // 飛行機
		{ new CDirect3DXFile(), "assets/skydome.x" } // スカイドーム
	};

	for (const auto& xfile : xfiles) xfile.first->LoadXFile(xfile.second, g_DXGrobj->GetDXDevice());

	skydome = new Data(xfiles[1].first);
	skydome->cor = { 0, 0, 0 };
	skydome->rot = { 0, 0, 0 };
	camera = new Camera();

	for (auto i = 0; i < sqrt(hikouki_count); ++i)
	{
		for (auto j = 0; j < sqrt(hikouki_count); ++j)
		{
			D3DXMATRIX mx;
			data.push_back(new Data(xfiles[0].first));
			data.back()->cor = { 0.0f, 0.0f, 1.0f };
			data.back()->rot = { rand() % 2 - 1.0f, rand() % 2 - 1.0f, rand() % 2 - 1.0f };
			MakeWorldMatrix(mx, data.back()->mat, { 0.0f, 0.0f, 0.0f }, { j * margin - margin, i * margin - margin,  0.0f });
		}
	}

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

	if (GetKeyboardTrigger(DIK_ADD) && now_controll + 1 < data.size()) now_controll++;
	if (GetKeyboardTrigger(DIK_SUBTRACT) && now_controll > 0) now_controll--;
	
	if (GetKeyboardTrigger(DIK_V)) tps = !tps;

	int keys[] = {
		DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9
	};

	for (auto i = 0; i < 9; i++) {
		if (GetKeyboardTrigger(keys[i]))
		{
			data[i]->explosion_flag = !data[i]->explosion_flag;
			if (data[i]->explosion_flag) {
				data[i]->explosion->TriangleTransforms(data[i]->mat);
				data[i]->explosion->Start();
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

	static auto last_update = std::chrono::system_clock::now();

	for (const auto& hikouki : data)
	{
		D3DXMATRIX mx;

		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_update).count() > 1)
		{
			hikouki->rot.x = rand() % 2 - 1;
			hikouki->rot.y = rand() % 2 - 1;
		}

		MakeWorldMatrix(mx, hikouki->mat, hikouki->rot, hikouki->cor);

		if (hikouki->explosion_flag) hikouki->explosion->Update();
	}

	if (tps) // tps
	{
		camera->looking_for = D3DXVECTOR3(data[now_controll]->mat._41, data[now_controll]->mat._42, data[now_controll]->mat._43);
		camera->looking_at = camera->looking_for - 10 * D3DXVECTOR3(data[now_controll]->mat._31, data[now_controll]->mat._32, data[now_controll]->mat._33);
		camera->up = D3DXVECTOR3(data[now_controll]->mat._21, data[now_controll]->mat._22, data[now_controll]->mat._23);
	}
	else // fps
	{
		camera->looking_at = 2 * D3DXVECTOR3(data[now_controll]->mat._41, data[now_controll]->mat._42, data[now_controll]->mat._43); // ずらさないと本体と被っちまうので
		camera->looking_for = camera->looking_at + 10 * D3DXVECTOR3(data[now_controll]->mat._31, data[now_controll]->mat._32, data[now_controll]->mat._33);
		camera->up = D3DXVECTOR3(data[now_controll]->mat._21, data[now_controll]->mat._22, data[now_controll]->mat._23);
	}
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameRender(){

	D3DXMATRIX view, proj, world;

	D3DXMatrixLookAtLH(&view, &camera->looking_at, &camera->looking_for, &camera->up);
												// カメラ行列を固定パイプラインへセット
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &view);

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(&proj,
		D3DX_PI / 2,					// 視野角
		(float)width / (float)height,	// アスペクト比
		1.0f,						// ニアプレーン
		5000.0f);					// ファープレーン

									// 射影変換行列を固定パイプラインへセット
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &proj);
	// Ｚバッファ有効
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ライト有効
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// カリング無効化
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// 環境光セット
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

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
			g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &hikouki->mat);	// ワールド変換行列をセット
			hikouki->xfile->Draw(g_DXGrobj->GetDXDevice());				// Ｘファイル描画
		}
	}

	D3DXMatrixIdentity(&world);
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &world);	// ワールド変換行列をセット
	skydome->xfile->Draw(g_DXGrobj->GetDXDevice());

	g_DXGrobj->GetDXDevice()->EndScene();	// 描画の終了を待つ

	if (g_DXGrobj->GetDXDevice()->Present(NULL, NULL, NULL, NULL) != D3D_OK){ // バックバッファからプライマリバッファへ転送
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
