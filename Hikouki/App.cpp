#include "App.h"

rxcpp::subjects::subject<const WPARAM> App::subject;

App::App(HINSTANCE _ins)
	: ins(_ins)
{
	if (!RegisterClassEx(&WNDCLASSEX
		{
			sizeof(WNDCLASSEX),
			0,
			static_cast<WNDPROC>(this->wndProc),
			0,
			0,
			ins,
			LoadIcon((HINSTANCE)NULL, IDI_APPLICATION),
			LoadCursor((HINSTANCE)NULL, IDC_ARROW),
			(HBRUSH)GetStockObject(WHITE_BRUSH),
			0,
			name,
			LoadIcon((HINSTANCE)NULL, IDI_WINLOGO)
		}
	)) throw "Couldn't register window class.";

	if (isFullscreen)
	{
		wh = CreateWindow(
			name,							// ウィンドウクラスの名前
			title,							// タイトル
			WS_VISIBLE | WS_POPUP,			// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横
			width, height,				// ウィンドウサイズ
			NULL,							// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			ins,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし
	}
	else
	{
		RECT	rWindow, rClient;

		wh = CreateWindow(
			name,							// ウィンドウクラスの名前
			title,							// タイトル
			WS_CAPTION | WS_SYSMENU,		// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横(あとで中央に移動させます)
			width, height, // ウィンドウサイズ
			HWND_DESKTOP,					// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			ins,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし

											// ウインドウサイズを再計算（Metricsだけでは、フレームデザインでクライアント領域サイズが変わってしまうので）
		GetWindowRect(wh, &rWindow);
		GetClientRect(wh, &rClient);
		auto _width = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + width;
		auto _height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + height;
		SetWindowPos(
			wh,
			NULL,
			GetSystemMetrics(SM_CXSCREEN) / 2 - _width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - _height / 2,
			_width - 1,
			_height - 1,
			SWP_NOZORDER);
	}

	if (!wh) throw "Error";
}

int App::run(int mode)
{
	// ウインドウを表示する
	ShowWindow(wh, mode);
	UpdateWindow(wh);

	try
	{
		auto gameController = new GameController(ins, wh, width, height, isFullscreen);
		bool isLoop = true;
		MSG msg;

		subject.get_observable().subscribe([&isLoop](const WPARAM)
		{
			isLoop = false;
		});

		while (isLoop)
		{	// メッセージ･ループ
			if (!GetMessage(&msg, NULL, 0, 0))
			{	// メッセージを取得
				break;
			}
			else
			{
				TranslateMessage(&msg); 			// 文字メッセージへのコンバート）
				DispatchMessage(&msg); 				// メッセージをWndProcへ送る
			}
		}

		// ゲーム終了フラグをセットする
		gameController->SetEnd();

		delete gameController;

		return static_cast<int>(msg.wParam);
	}
	catch (const char *err)
	{
		MessageBox(wh, err, "ERROR", MB_OK);
		return 0;
	}
	catch (std::runtime_error e)
	{
		MessageBox(wh, e.what(), "ERROR", MB_OK);
		return 0;
	}
}

LRESULT App::wndProc(HWND wh, UINT message, WPARAM w, LPARAM l)
{
	switch (message) {
	case WM_KEYDOWN:
		switch (w) {
		case VK_ESCAPE:
			DestroyWindow(wh);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		subject.get_subscriber().on_next(w);
		break;
	default:
		return DefWindowProc(wh, message, w, l);
	}
	return 0;
}
