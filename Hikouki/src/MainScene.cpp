#include "MainScene.h"
#include "Plain2D.h"
#include <algorithm>

MainScene::MainScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer), under_controll(0), cam_types(trau::CameraTypes::OVER)
{
	cameras.fps = std::unique_ptr<trau::FPSCamera>(new trau::FPSCamera());
	cameras.tps = std::unique_ptr<trau::TPSCamera>(new trau::TPSCamera());
	cameras.over = std::unique_ptr<trau::OverCamera>(new trau::OverCamera());

	xfile_manager = new XFileManager(graphics->GetDXDevice());
	xfile_manager->add({
		{ "Airplain", "assets/f1.x" }, // 飛行機
		{ "Skydome", "assets/skydome.x" } // スカイドーム
		});

	airplains.emplace_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3(0.0, 0.0, 10.0f), timer));
	airplains.emplace_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3(0.0, 0.0, -10.0f), timer));
	text_areas.emplace_back(new trau::TextArea(graphics->GetDXDevice(), 0, 0, std::string("こんにちは")));

	// ブチ込め
	for (const auto& airplain : airplains) gameObjects.emplace_back(airplain);
	for (const auto& text_area : text_areas) gameObjects.emplace_back(text_area);
	gameObjects.emplace_back(new XFileObjectBase(xfile_manager->get("Skydome")));

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
}

Scene::State MainScene::exec()
{
	timer->run([&] {
		sortGameObjectsPriority();
		input(); // 入力
		update(); // 更新
		sortGameObjectsLayer();
		render(); // 描画
	});
	return state;
}

Scene::State MainScene::getState()
{
	return state;
}

void MainScene::sortGameObjectsLayer()
{
	// 描画順なので降順にする
	std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b) {
		return a->getLayer() > b->getLayer();
	});
}

void MainScene::sortGameObjectsPriority()
{
	// update処理順なので昇順にする
	std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b) {
		return a->getPriority() < b->getPriority();
	});
}

void MainScene::input()
{
	try
	{
		inputDevice->update();
	}
	catch (const char* e)
	{

	}

	auto mouse_current_state = inputDevice->getMouseState();

	if (inputDevice->getTrigger(KeyCode::V)) cam_types++;

	if (inputDevice->getTrigger(KeyCode::Add) && under_controll + 1 < airplains.size()) under_controll++;
	if (inputDevice->getTrigger(KeyCode::Subtract) && under_controll > 0) under_controll--;

	if (cam_types == trau::CameraTypes::OVER)
	{
		if (inputDevice->getPress(KeyCode::UpArrow)) cameras.over->elevation += 10.0f * timer->getMs();
		if (inputDevice->getPress(KeyCode::DownArrow)) cameras.over->elevation -= 10.0f * timer->getMs();
		if (inputDevice->getPress(KeyCode::RightArrow)) cameras.over->azimuth -= 10.0f * timer->getMs();
		if (inputDevice->getPress(KeyCode::LeftArrow)) cameras.over->azimuth += 10.0f * timer->getMs();
		if (inputDevice->getPress(KeyCode::Return) && 0 < cameras.over->distance) cameras.over->distance -= 0.1f;
		if (inputDevice->getPress(KeyCode::BackSpace)) cameras.over->distance += 0.1f;
		cameras.over->distance -= mouse_current_state.lZ / 10;
	}

	if (inputDevice->getTrigger(KeyCode::Numpad5)) airplains[under_controll]->switchExplosion();
	if (inputDevice->getTrigger(KeyCode::Numpad8)) airplains[under_controll]->switchDrawBBox();
	if (inputDevice->getTrigger(KeyCode::Numpad6)) airplains[under_controll]->addTrans(D3DXVECTOR3{ 0, 0, -10 });
	if (inputDevice->getTrigger(KeyCode::Numpad4)) airplains[under_controll]->addTrans(D3DXVECTOR3{ 0, 0, 10 });
	if (inputDevice->getTrigger(KeyCode::Space)) airplains[under_controll]->setTrans(D3DXVECTOR3{ 0, 0, 0 });

	if (inputDevice->getTrigger(KeyCode::Return)) state = State::Exit;
}

void MainScene::update()
{
	for (const auto& gameObject : gameObjects) gameObject->update();

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

void MainScene::render()
{
	graphics->SetCamera([&](const LPDIRECT3DDEVICE9 device) {
		if (cam_types == trau::CameraTypes::FPS) cameras.fps->set(device);
		else if (cam_types == trau::CameraTypes::TPS) cameras.tps->set(device);
		else if (cam_types == trau::CameraTypes::OVER) cameras.over->set(device);
	});

	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& gameObject : gameObjects) gameObject->draw(device);
	});
}
