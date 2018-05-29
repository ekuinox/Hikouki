#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: end(false), under_controll(0), over_camera({ 0, 90, -100 }), view_type(CameraTypes::OVER)
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
	input_device = new Input(hinst);
	keyboard = new Keyboard(input_device->get(), hwnd);
	mouse = new Mouse(input_device->get(), hwnd);

#ifdef _DEBUG
//	DebugConsole::create_console_window();
#endif

	graphics = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成

	if (!graphics->Init(hwnd, fullscreen, width, height)) {	// DirectX Graphicsオブジェクト初期化
		throw "DirectX 初期化エラー";
	}

	constexpr float margin = 50.0f;

	camera = new Camera();

	xfile_manager = new XFileManager(graphics->GetDXDevice());
	xfile_manager->add({
		{ "Airplain", "assets/f1.x" }, // 飛行機
		{ "Skydome", "assets/skydome.x" } // スカイドーム
		});

	skydome = new XFileObjectBase(xfile_manager->get("Skydome"));

	airplains.push_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3( 0.0, 0.0, 10.0f )));
	airplains.push_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3( 0.0, 0.0, -10.0f)));


	// イベントハンドル生成
	event_handle = CreateEvent(NULL, false, false, NULL);
	if (event_handle == NULL) {
		throw "CreateEvent エラー";
	}
	// スレッド生成(ゲームメイン)
	main_thread = std::thread([&](){ main(); });

#ifdef _DEBUG
	debug_font = new CDebugFont();
	debug_font->CreateFontA(graphics->GetDXDevice());
#endif
}

void GameController::uninit()
{
	main_thread.join();

	CloseHandle(event_handle);

	if (graphics != NULL) {
		graphics->Exit();
		delete graphics;
	}

	delete keyboard;
	delete input_device;
	delete mouse;
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
	keyboard->update();
	try
	{
		mouse->update();
	}
	catch (const char *e)
	{
#ifdef _DEBUG
		std::cout << e << std::endl;
#endif
	}
	auto mouse_current_state = mouse->getState();

	if (keyboard->getTrigger(DIK_ADD) && under_controll + 1 < airplains.size()) under_controll++;
	if (keyboard->getTrigger(DIK_SUBTRACT) && under_controll > 0) under_controll--;

	if (view_type == CameraTypes::OVER)
	{
		if (keyboard->getPress(DIK_UPARROW)) over_camera.elevation += 0.1f;
		if (keyboard->getPress(DIK_DOWNARROW)) over_camera.elevation -= 0.1f;
		if (keyboard->getPress(DIK_RIGHTARROW)) over_camera.azimuth -= 0.1f;
		if (keyboard->getPress(DIK_LEFTARROW)) over_camera.azimuth += 0.1f;
		if (keyboard->getPress(DIK_RETURN) && 0 < over_camera.distance) over_camera.distance -= 1.0f;
		if (keyboard->getPress(DIK_BACKSPACE)) over_camera.distance += 1.0f;
		over_camera.distance -= mouse_current_state.lZ / 10;
	}

	if (keyboard->getTrigger(DIK_V)) view_type++;
	if (keyboard->getTrigger(DIK_NUMPAD5)) airplains[under_controll]->switchExplosion();
	if (keyboard->getTrigger(DIK_NUMPAD8)) airplains[under_controll]->switchDrawBBox();
	if (keyboard->getTrigger(DIK_NUMPAD6)) airplains[under_controll]->addTrans({ 0, 0, -1 });
	if (keyboard->getTrigger(DIK_NUMPAD4)) airplains[under_controll]->addTrans({ 0, 0, 1 });
	if (keyboard->getTrigger(DIK_SPACE)) airplains[under_controll]->setTrans({0, 0, 0});

#ifdef _DEBUG
	printf("%ld, %ld, %ld\n", mouse_current_state.lX, mouse_current_state.lY, mouse_current_state.lZ);
#endif
}

void GameController::update()
{
	for (const auto& airplain : airplains)
	{
		airplain->update();
	}

	auto colls = getCollisions({ airplains[0]->getBBox() }, { airplains[1]->getBBox() });
	if (colls.size() > 0)
//	if (airplains[0]->getBBox()->isCollision(airplains[1]->getBBox()))
//	if (isCollision(airplains[0]->getBBox(), airplains[1]->getBBox()))
	{
#ifdef _DEBUG
		debug_text = "あたっとる";
#endif // DEBUG
	}
	else
	{
#ifdef _DEBUG
		debug_text = "あたっとらん";
#endif // DEBUG
	}

	D3DXMATRIX mat;

	switch (view_type)
	{
	case CameraTypes::OVER:
#define OVER_CAMERA_FLAG 1
#if OVER_CAMERA_FLAG == 1
		camera->look_at = D3DXVECTOR3(0, 0, 0);
		camera->eye = D3DXVECTOR3(
			over_camera.distance * sin(over_camera.elevation) * cos(over_camera.azimuth),
			over_camera.distance * cos(over_camera.elevation),
			over_camera.distance * sin(over_camera.azimuth) * sin(over_camera.elevation)
		);
		camera->eye += camera->look_at;
		camera->up = D3DXVECTOR3(0, 1, 0);
#elif OVER_CAMERA_FLAG == 2
		camera->look_at = D3DXVECTOR3(0, 0, 0);
		camera->eye = D3DXVECTOR3(
			over_camera.distance * sin(over_camera.elevation) * cos(over_camera.azimuth),
			over_camera.distance * cos(over_camera.elevation),
			over_camera.distance * sin(over_camera.azimuth) * sin(over_camera.elevation)
		);
		D3DXMatrixInverse(&mat, NULL, &view);
		camera->up = D3DXVECTOR3(
			mat._21,
			mat._22,
			mat._23
		);
#endif
		break;
	case CameraTypes::TPS:
		camera->look_at = D3DXVECTOR3(airplains[under_controll]->getMat()._41, airplains[under_controll]->getMat()._42, airplains[under_controll]->getMat()._43);
		camera->eye = camera->look_at - 10 * D3DXVECTOR3(airplains[under_controll]->getMat()._31, airplains[under_controll]->getMat()._32, airplains[under_controll]->getMat()._33);
		camera->up = D3DXVECTOR3(airplains[under_controll]->getMat()._21, airplains[under_controll]->getMat()._22, airplains[under_controll]->getMat()._23);
		break;
	case CameraTypes::FPS:
		camera->eye = 2 * D3DXVECTOR3(airplains[under_controll]->getMat()._41, airplains[under_controll]->getMat()._42, airplains[under_controll]->getMat()._43); // ずらさないと本体と被っちまうので
		camera->look_at = camera->eye + 10 * D3DXVECTOR3(airplains[under_controll]->getMat()._31, airplains[under_controll]->getMat()._32, airplains[under_controll]->getMat()._33);
		camera->up = D3DXVECTOR3(airplains[under_controll]->getMat()._21, airplains[under_controll]->getMat()._22, airplains[under_controll]->getMat()._23);
		break;
	}
}

void GameController::render()
{
	D3DXMATRIX proj, world;

	D3DXMatrixLookAtLH(&view, &camera->eye, &camera->look_at, &camera->up);
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

#ifdef _DEBUG
	debug_font->DrawTextA(0, 0, debug_text.c_str());
#endif // _DEBUG


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
