#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: under_controll(0), cam_types(trau::CameraTypes::OVER)
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

	graphics = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成

	if (!graphics->Init(hwnd, fullscreen, width, height)) {	// DirectX Graphicsオブジェクト初期化
		throw "DirectX 初期化エラー";
	}

	cameras.fps = std::unique_ptr<trau::FPSCamera>(new trau::FPSCamera());
	cameras.tps = std::unique_ptr<trau::TPSCamera>(new trau::TPSCamera());
	cameras.over = std::unique_ptr<trau::OverCamera>(new trau::OverCamera());
	
	timer = new trau::Timer();

	xfile_manager = new XFileManager(graphics->GetDXDevice());
	xfile_manager->add({
		{ "Airplain", "assets/f1.x" }, // 飛行機
		{ "Skydome", "assets/skydome.x" } // スカイドーム
		});

	airplains.emplace_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3(0.0, 0.0, 10.0f), timer));
	airplains.emplace_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3(0.0, 0.0, -10.0f), timer));
	text_areas.emplace_back(new trau::TextArea(graphics->GetDXDevice(), 0, 0, std::string("こんにちは")));

	// ブチ込め
	for (const auto& airplain : airplains) game_objects.emplace_back(airplain);
	for (const auto& text_area : text_areas) game_objects.emplace_back(text_area);
	game_objects.emplace_back(new XFileObjectBase(xfile_manager->get("Skydome")));

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
	timer->run([&] {
		input(); // 入力
		update(); // 更新
		render(); // 描画
	});
}

void GameController::input()
{
	try
	{
		input_device->update();
	}
	catch (const char* e)
	{

	}

	auto mouse_current_state = input_device->getMouseState();

	if (input_device->getTrigger(KeyCode::V)) cam_types++;

	if (input_device->getTrigger(KeyCode::Add) && under_controll + 1 < airplains.size()) under_controll++;
	if (input_device->getTrigger(KeyCode::Subtract) && under_controll > 0) under_controll--;

	if (cam_types == trau::CameraTypes::OVER)
	{
		if (input_device->getPress(KeyCode::UpArrow)) cameras.over->elevation += 10.0f * timer->getMs();
		if (input_device->getPress(KeyCode::DownArrow)) cameras.over->elevation -= 10.0f * timer->getMs();
		if (input_device->getPress(KeyCode::RightArrow)) cameras.over->azimuth -= 10.0f * timer->getMs();
		if (input_device->getPress(KeyCode::LeftArrow)) cameras.over->azimuth += 10.0f * timer->getMs();
		if (input_device->getPress(KeyCode::Return) && 0 < cameras.over->distance) cameras.over->distance -= 0.1f;
		if (input_device->getPress(KeyCode::BackSpace)) cameras.over->distance += 0.1f;
		cameras.over->distance -= mouse_current_state.lZ / 10;
	}
	
	if (input_device->getTrigger(KeyCode::Numpad5)) airplains[under_controll]->switchExplosion();
	if (input_device->getTrigger(KeyCode::Numpad8)) airplains[under_controll]->switchDrawBBox();
	if (input_device->getTrigger(KeyCode::Numpad6)) airplains[under_controll]->addTrans(D3DXVECTOR3{ 0, 0, -10 });
	if (input_device->getTrigger(KeyCode::Numpad4)) airplains[under_controll]->addTrans(D3DXVECTOR3{ 0, 0, 10 });
	if (input_device->getTrigger(KeyCode::Space)) airplains[under_controll]->setTrans(D3DXVECTOR3{0, 0, 0});
}

void GameController::update()
{
	for (const auto& game_object : game_objects) game_object->update();

	auto colls = getCollisions({ airplains[0]->getBBox() }, { airplains[1]->getBBox() });
	
	text_areas.front()->text = "{\n    Airplain:\n    {\n";
	for (auto i = 0; i < 2; ++i)
	{
		auto pos = airplains[i]->getBBox()->getPosition();
		text_areas.front()->text += (boost::format(
			"        { X: %2%, Y: %3%, Z: %4%, R: %5%, Hit: %6% }%7%\n"
		) % i % pos.x % pos.y % pos.z % airplains[i]->getBBox()->getR() % (colls.size() > 0 ? "TRUE" : "FALSE") % (i == 1 ? "" : ",")).str();
	}
	text_areas.front()->text += (boost::format(
		"    },\n    Distance: %1%\n} \n%2%\n"
	) % calculateDistance(airplains[0]->getBBox()->getPosition(), airplains[1]->getBBox()->getPosition()) % timer->getMs()).str();

	D3DXMATRIX mat;

	cameras.tps->update(airplains[under_controll]->getMat());
	cameras.fps->update(airplains[under_controll]->getMat());
	cameras.over->update();
}

void GameController::render()
{
	graphics->SetCamera([&](const LPDIRECT3DDEVICE9 device) {
		if (cam_types == trau::CameraTypes::FPS) cameras.fps->set(device);
		else if (cam_types == trau::CameraTypes::TPS) cameras.tps->set(device);
		else if (cam_types == trau::CameraTypes::OVER) cameras.over->set(device);
	});
		
	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& game_object : game_objects) game_object->draw(device);
	});
}