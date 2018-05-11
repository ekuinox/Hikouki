#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: end(false), under_controll(0)
{
	init(hinst, hwnd, _width, _height, fullscreen);
}

GameController::~GameController()
{
	uninit();
}

void GameController::init(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
{
	width = _width;
	height = _height;

	// 入力初期化
	InitKeyboard(hinst, hwnd);

	graphics = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成

	if (!graphics->Init(hwnd, fullscreen, width, height)) {	// DirectX Graphicsオブジェクト初期化
		throw "DirectX 初期化エラー";
	}

	constexpr int hikouki_count = 9;
	constexpr float margin = 50.0f;

	camera = new Camera();

	xfile_manager = new XFileManager(graphics->GetDXDevice());
	xfile_manager->add({
		{ "Airplain", "assets/f1.x" }, // 飛行機
		{ "Skydome", "assets/skydome.x" } // スカイドーム
		});

	skydome = new XFileObjectBase(xfile_manager->get("Skydome"));

	for (auto i = 0; i < sqrt(hikouki_count); ++i)
	{
		for (auto j = 0; j < sqrt(hikouki_count); ++j)
		{
			airplains.push_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice()));
		}
	}

	// イベントハンドル生成
	event_handle = CreateEvent(NULL, false, false, NULL);
	if (event_handle == NULL) {
		throw "CreateEvent エラー";
	}
	// スレッド生成(ゲームメイン)
	main_thread = std::thread([&](){ main(); });
}

void GameController::uninit()
{
	main_thread.join();

	CloseHandle(event_handle);

	if (graphics != NULL) {
		graphics->Exit();
		delete graphics;
	}
}

void GameController::main()
{
	while (1) {
		auto sts = WaitForSingleObject(event_handle, 1000);	// イベントフラグがセットされるのを待つ（1000msでタイムアウト）
		if (sts == WAIT_FAILED) {
			break;
		}
		else if (sts == WAIT_TIMEOUT) {
			if (end) {
				break;
			}
			continue;
		}

		input(); // 入力
		update(); // 更新
		render(); // 描画
	}
}

void GameController::input()
{
	UpdateInput();

	if (GetKeyboardTrigger(DIK_ADD) && under_controll + 1 < airplains.size()) under_controll++;
	if (GetKeyboardTrigger(DIK_SUBTRACT) && under_controll > 0) under_controll--;

	if (GetKeyboardTrigger(DIK_V)) tps = !tps;

	int keys[] = {
		DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9
	};

	for (auto i = 0; i < 9; i++) {
		if (GetKeyboardTrigger(keys[i]))
		{
			airplains[i]->switchExplosion();
		}
	}
}

void GameController::update()
{
	for (const auto& airplain : airplains)
	{
		airplain->update();
	}

	if (tps) // tps
	{
		camera->looking_for = D3DXVECTOR3(airplains[under_controll]->getMat()._41, airplains[under_controll]->getMat()._42, airplains[under_controll]->getMat()._43);
		camera->looking_at = camera->looking_for - 10 * D3DXVECTOR3(airplains[under_controll]->getMat()._31, airplains[under_controll]->getMat()._32, airplains[under_controll]->getMat()._33);
		camera->up = D3DXVECTOR3(airplains[under_controll]->getMat()._21, airplains[under_controll]->getMat()._22, airplains[under_controll]->getMat()._23);
	}
	else // fps
	{
		camera->looking_at = 2 * D3DXVECTOR3(airplains[under_controll]->getMat()._41, airplains[under_controll]->getMat()._42, airplains[under_controll]->getMat()._43); // ずらさないと本体と被っちまうので
		camera->looking_for = camera->looking_at + 10 * D3DXVECTOR3(airplains[under_controll]->getMat()._31, airplains[under_controll]->getMat()._32, airplains[under_controll]->getMat()._33);
		camera->up = D3DXVECTOR3(airplains[under_controll]->getMat()._21, airplains[under_controll]->getMat()._22, airplains[under_controll]->getMat()._23);
	}
}

void GameController::render()
{
	D3DXMATRIX view, proj, world;

	D3DXMatrixLookAtLH(&view, &camera->looking_at, &camera->looking_for, &camera->up);
	// カメラ行列を固定パイプラインへセット
	graphics->GetDXDevice()->SetTransform(D3DTS_VIEW, &view);

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(&proj,
		D3DX_PI / 2,					// 視野角
		(float)width / (float)height,	// アスペクト比
		1.0f,						// ニアプレーン
		5000.0f);					// ファープレーン

									// 射影変換行列を固定パイプラインへセット
	graphics->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &proj);
	// Ｚバッファ有効
	graphics->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ライト有効
	graphics->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// カリング無効化
	graphics->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// 環境光セット
	graphics->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	// ターゲットバッファのクリア、Ｚバッファのクリア
	graphics->GetDXDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// 描画の開始をDirectXに通知
	graphics->GetDXDevice()->BeginScene();

	for (const auto& airplain : airplains) airplain->draw(graphics->GetDXDevice());
	skydome->draw(graphics->GetDXDevice());

	graphics->GetDXDevice()->EndScene();	// 描画の終了を待つ

	if (graphics->GetDXDevice()->Present(NULL, NULL, NULL, NULL) != D3D_OK) { // バックバッファからプライマリバッファへ転送
		graphics->GetDXDevice()->Reset(&graphics->GetDXD3dpp());
	}
}

void GameController::setEvent()
{
	if (!end) SetEvent(event_handle);
}

void GameController::setEndFlag()
{
	end = true;
}
