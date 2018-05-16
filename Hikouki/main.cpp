//*****************************************************************************
//!	@file	main.cpp
//!	@brief	
//!	@note	GM31ひな形メイン
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <thread>
#include "GameController.h"

//-----------------------------------------------------------------------------
// マクロの定義
//-----------------------------------------------------------------------------
#define		NAME			"win32A"
#define		TITLE			"GM31 ひながた"
#define		SCREEN_X		1280
#define		SCREEN_Y		720
#define		FULLSCREEN		0				// フルスクリーンフラグ

//-----------------------------------------------------------------------------
// リンクライブラリの設定
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")			// リンク対象ライブラリにd3d9.libを追加
#pragma comment(lib, "d3dx9.lib")			// リンク対象ライブラリにd3dx9.libを追加
#pragma comment(lib, "winmm.lib")			// リンク対象ライブラリにwinmm.libを追加

//-----------------------------------------------------------------------------
// プロトタイプ宣言
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);			// ウインドウプロシージャ
void	CALLBACK TimerProc(UINT ,UINT,DWORD,DWORD,DWORD);			// タイマ処理

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
int				g_timerid=0;		// タイマＩＤ
GameController *game_controller;

//==============================================================================
//!	@fn		WinMain
//!	@brief	エントリポイント
//!	@param	インスタンスハンドル
//!	@param	意味なし
//!	@param	起動時の引数文字列
//!	@param	ウインドウ表示モード
//!	@retval	TRUE　成功終了/FALSE　失敗終了
//!	@note	
//==============================================================================
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// アプリケーションのハンドル
					 HINSTANCE 	hPrevInstance,	// 今のWindowsバージョンでは使われない
					 LPSTR 		lpszArgs, 		// 起動時の引数（文字列）
					 int 		nWinMode)		// ウインドウ表示モード
{
	HWND			hwnd;						// ウインドウハンドル
	MSG				msg;						// メッセージ構造体
	WNDCLASSEX		wcex;						// ウインドウクラス構造体
	int				width = SCREEN_X;			// ウインドウの幅 計算用ワーク
	int				height = SCREEN_Y;			// ウインドウの高さ 計算用ワーク

	// ウインドウクラス情報のセット
	wcex.hInstance		= hInstance;			// インスタンス値のセット
	wcex.lpszClassName	= NAME;					// クラス名
	wcex.lpfnWndProc	= (WNDPROC)WndProc;		// ウインドウメッセージ関数
	wcex.style			= 0;					// ウインドウスタイル
	wcex.cbSize 		= sizeof(WNDCLASSEX);	// 構造体のサイズ
	wcex.hIcon			= LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ラージアイコン
	wcex.hIconSm		= LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// スモールアイコン
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// カーソルスタイル
	wcex.lpszMenuName	= 0; 					// メニューなし
	wcex.cbClsExtra		= 0;					// エキストラなし
	wcex.cbWndExtra		= 0;					
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);		// 背景色白

	if (!RegisterClassEx(&wcex)) return FALSE;	// ウインドウクラスの登録

	if(FULLSCREEN){
		hwnd = CreateWindow(
			NAME,							// ウィンドウクラスの名前
			TITLE,							// タイトル
			WS_VISIBLE | WS_POPUP,			// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横
			SCREEN_X, SCREEN_Y,				// ウィンドウサイズ
			NULL,							// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			hInstance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし
	}else{
		RECT	rWindow, rClient;

		hwnd = CreateWindow(
			NAME,							// ウィンドウクラスの名前
			TITLE,							// タイトル
			WS_CAPTION | WS_SYSMENU,		// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横(あとで中央に移動させます)
			SCREEN_X, SCREEN_Y,				// ウィンドウサイズ
			HWND_DESKTOP,					// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			hInstance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし
		
		// ウインドウサイズを再計算（Metricsだけでは、フレームデザインでクライアント領域サイズが変わってしまうので）
		GetWindowRect(hwnd, &rWindow);
		GetClientRect(hwnd, &rClient);
		width  = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + SCREEN_X;
		height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + SCREEN_Y;
		SetWindowPos(
			hwnd,
			NULL,
			GetSystemMetrics(SM_CXSCREEN)/2-width/2,
			GetSystemMetrics(SM_CYSCREEN)/2-height/2,
			width-1,
			height-1,
			SWP_NOZORDER);
	}

	if (!hwnd) return FALSE;

	// ウインドウを表示する
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	try{
		game_controller = new GameController(hInstance, hwnd, SCREEN_X, SCREEN_Y, FULLSCREEN);
	} catch (const char *err) {
		MessageBox(hwnd, "ERROR", err, MB_OK);
	}

	// イベントタイマーをセットする
	timeBeginPeriod(1); // タイマの分解能力を１ｍｓにする
	g_timerid = timeSetEvent(16, 1, TimerProc, 1, TIME_PERIODIC);

	while(1){	// メッセージ･ループ
		if( !GetMessage(&msg, NULL, 0, 0) ){	// メッセージを取得
			break; 
		}else{
			TranslateMessage(&msg); 			// 文字メッセージへのコンバート）
			DispatchMessage(&msg); 				// メッセージをWndProcへ送る
		}
	}

	// ゲーム終了フラグをセットする
	game_controller->setEndFlag();

	if( g_timerid ) timeKillEvent(g_timerid);	// マルチメディアタイマの終了
	timeEndPeriod(1);							// タイマの分解能力もとに戻す

	delete game_controller;
	return (int)msg.wParam;
}

//==============================================================================
//!	@fn		WindowProc
//!	@brief	ウインドウプロシージャ
//!	@param	ウインドウハンドル
//!	@param	メッセージ
//!	@param	Wパラメータ
//!	@param	Lパラメータ
//!	@retval	終了状況
//==============================================================================
LRESULT WINAPI WndProc(	HWND hwnd, 		// ウィンドウハンドル
						UINT message,	// メッセージ識別子
						WPARAM wParam,	// 付帯情報１
						LPARAM lParam)	// 付帯情報２
{
	switch( message ){
	case WM_KEYDOWN:
		switch(wParam){
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

//==============================================================================
//!	@fn		TimerProc
//!	@brief	１６ｍｓ毎にイベントオブジェクトをセットする
//!	@param	タイマＩＤ
//!	@param	未使用
//!	@param	未使用
//!	@param	未使用
//!	@param	未使用
//!	@retval	なし
//==============================================================================
void CALLBACK TimerProc(UINT, UINT, DWORD, DWORD, DWORD)
{
	game_controller->setEvent();
}

//******************************************************************************
//	End of file.
//******************************************************************************
