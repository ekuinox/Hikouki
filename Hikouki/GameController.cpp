#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: under_controll(0), over_camera({ 0, 90, -100 }), view_type(CameraTypes::OVER)
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
	input_device = new Input(hinst, hwnd);


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

	// 初期設定
	graphics->SetRenderStateArray({
		{ D3DRS_ZENABLE, TRUE }, // Ｚバッファ有効
		{ D3DRS_LIGHTING, true }, // ライト有効
		{ D3DRS_CULLMODE, D3DCULL_NONE }, // カリング無効化
		{ D3DRS_AMBIENT, 0xffffffff }, // 環境光セット
		{ D3DRS_ALPHABLENDENABLE, TRUE }, // アルファ・ブレンディングを行う
		{ D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA }, // 透過処理を行う
		{ D3DRS_SRCBLEND, D3DBLEND_SRCALPHA } // 半透明処理を行う
		});

#ifdef _DEBUG
	debug_font = new CDebugFont();
	debug_font->CreateFontA(graphics->GetDXDevice());
#endif
	Start();
}

void GameController::uninit()
{
	Stop();

	if (graphics != NULL) {
		graphics->Exit();
		delete graphics;
	}

	delete input_device;
}

void GameController::main()
{
	input(); // 入力
	update(); // 更新
	render(); // 描画
}

void GameController::input()
{
	input_device->update();

	auto mouse_current_state = input_device->getMouseState();

	if (input_device->getTrigger(KeyCode::Add) && under_controll + 1 < airplains.size()) under_controll++;
	if (input_device->getTrigger(KeyCode::Subtract) && under_controll > 0) under_controll--;

	if (view_type == CameraTypes::OVER)
	{
		if (input_device->getPress(KeyCode::UpArrow)) over_camera.elevation += 0.01f;
		if (input_device->getPress(KeyCode::DownArrow)) over_camera.elevation -= 0.01f;
		if (input_device->getPress(KeyCode::RightArrow)) over_camera.azimuth -= 0.01f;
		if (input_device->getPress(KeyCode::LeftArrow)) over_camera.azimuth += 0.01f;
		if (input_device->getPress(KeyCode::Return) && 0 < over_camera.distance) over_camera.distance -= 0.1f;
		if (input_device->getPress(KeyCode::BackSpace)) over_camera.distance += 0.1f;
		over_camera.distance -= mouse_current_state.lZ / 10;
	}

	if (input_device->getTrigger(KeyCode::V)) view_type++;
	if (input_device->getTrigger(KeyCode::Numpad5)) airplains[under_controll]->switchExplosion();
	if (input_device->getTrigger(KeyCode::Numpad8)) airplains[under_controll]->switchDrawBBox();
	if (input_device->getTrigger(KeyCode::Numpad6)) airplains[under_controll]->addTrans(D3DXVECTOR3{ 0, 0, -1 });
	if (input_device->getTrigger(KeyCode::Numpad4)) airplains[under_controll]->addTrans(D3DXVECTOR3{ 0, 0, 1 });
	if (input_device->getTrigger(KeyCode::Space)) airplains[under_controll]->setTrans(D3DXVECTOR3{0, 0, 0});

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
#ifdef _DEBUG
	debug_text = "{\n    BBoxes:\n    {\n";
	for (auto i = 0; i < 2; ++i)
	{
		auto pos = airplains[i]->getBBox()->getPosition();
		char line[255];
		sprintf(line, "        Airplain[%d]: { X: %f, Y: %f, Z: %f, R: %f, Hit: %s }%s\n", i, pos.x, pos.y, pos.z, airplains[i]->getBBox()->getR(), colls.size() > 0 ? "TRUE" : "FALSE", i == 1 ? "" : ",");
	//	debug_text = fmt::format("Airplain[{%d}] -> x: {%f}, y: {%f}, z: {%f}, r: {%f}, Hit: {%s}\n", i, pos.x, pos.y, pos.z, airplains[i]->getBBox()->getR(), colls.size() > 0 ? "TRUE" : "FALSE");
		debug_text += line;
	}
	debug_text += "    },\n";
	char line[255];
	sprintf(line, "    Distance: %f\n} ", calculateDistance(airplains[0]->getBBox()->getPosition(), airplains[1]->getBBox()->getPosition()));
	debug_text += line;
#endif

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

	D3DXMatrixLookAtLH(&view, &camera->eye, &camera->look_at, &camera->up);

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 2,					// 視野角
		(float)width / (float)height,	// アスペクト比
		1.0f,							// ニアプレーン
		5000.0f							// ファープレーン
	);
}

void GameController::render()
{
	graphics->SetView(view); // カメラ行列を固定パイプラインへセット
	graphics->SetProjection(proj); // 射影変換行列を固定パイプラインへセット
	graphics->Render([&](LPDIRECT3DDEVICE9 device) {
		for (const auto& airplain : airplains) airplain->draw(device);
		skydome->draw(device);
#ifdef _DEBUG
		debug_font->DrawTextA(0, 0, debug_text.c_str());
#endif // _DEBUG
	});
}