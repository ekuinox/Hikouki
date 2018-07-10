#include "MainScene.h"
#include <algorithm>
#include "../GameObject/EnemyAirplane.h"
#include "../GameObject/HomingMissile.h"
#include "../GameObjectAttachments/Collider.h"

MainScene::MainScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer), underControll(0), camType(trau::CameraTypes::OVER)
{
	cameras.fps = std::unique_ptr<trau::FPSCamera>(new trau::FPSCamera());
	cameras.tps = std::unique_ptr<trau::TPSCamera>(new trau::TPSCamera());
	cameras.over = std::unique_ptr<trau::OverCamera>(new trau::OverCamera());

	xFileManager = new XFileManager(graphics->GetDXDevice());
	xFileManager->add({
		{ "Airplane", "assets/f1.x" }, // 飛行機
		{ "Skydome", "assets/skydome.x" } // スカイドーム
		});

	airplanes.emplace_back(new Airplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), D3DXVECTOR3(0.0, 0.0, 10.0f)));
	airplanes.emplace_back(new Airplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), D3DXVECTOR3(0.0, 0.0, -10.0f)));
	textAreas.emplace_back(new trau::TextArea(graphics->GetDXDevice(), 0, 0, std::string("こんにちは")));
	textAreas.emplace_back(new trau::TextArea(graphics->GetDXDevice(), graphics->GetWidth() - 100, 0, std::string("")));

	// ブチ込め
	for (const auto& airplane : airplanes) gameObjects.emplace_back(airplane);
	for (const auto& text_area : textAreas) gameObjects.emplace_back(text_area);
	gameObjects.emplace_back(new XFileObjectBase(xFileManager->get("Skydome")));
	const auto& enemyAirplane = std::shared_ptr<EnemyAirplane>(new EnemyAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), "assets/GameObjectConfig/enemy.json"));
	gameObjects.emplace_back(enemyAirplane);
	gameObjects.emplace_back(new HomingMissile(xFileManager->get("Airplane"), enemyAirplane, 360.0f, D3DXVECTOR3{0, -20, 0}, D3DXVECTOR3{ 0, 0, 1 }));

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
	std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObjectInterface>& a, const std::shared_ptr<GameObjectInterface>& b) {
		return a->getLayer() > b->getLayer();
	});
}

void MainScene::sortGameObjectsPriority()
{
	// update処理順なので昇順にする
	std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObjectInterface>& a, const std::shared_ptr<GameObjectInterface>& b) {
		return a->getPriority() < b->getPriority();
	});
}

void MainScene::input()
{
	try
	{
		inputDevice->update();

		auto mouseCurrentState = inputDevice->getMouseState();

		if (inputDevice->getTrigger(KeyCode::V)) camType++;

		if (inputDevice->getTrigger(KeyCode::Add) && underControll + 1 < airplanes.size()) underControll++;
		if (inputDevice->getTrigger(KeyCode::Subtract) && underControll > 0) underControll--;

		if (camType == trau::CameraTypes::OVER)
		{
			if (inputDevice->getPress(KeyCode::UpArrow)) cameras.over->elevation += 10.0f * timer->getSeconds();
			if (inputDevice->getPress(KeyCode::DownArrow)) cameras.over->elevation -= 10.0f * timer->getSeconds();
			if (inputDevice->getPress(KeyCode::RightArrow)) cameras.over->azimuth -= 10.0f * timer->getSeconds();
			if (inputDevice->getPress(KeyCode::LeftArrow)) cameras.over->azimuth += 10.0f * timer->getSeconds();
			if (inputDevice->getPress(KeyCode::Return) && 0 < cameras.over->distance) cameras.over->distance -= 0.1f;
			if (inputDevice->getPress(KeyCode::BackSpace)) cameras.over->distance += 0.1f;
			cameras.over->distance -= mouseCurrentState.lZ / 10;
		}

		if (inputDevice->getTrigger(KeyCode::Numpad5)) airplanes[underControll]->switchExplosion();
		if (inputDevice->getTrigger(KeyCode::Numpad8)) airplanes[underControll]->switchDrawBBox();
		if (inputDevice->getTrigger(KeyCode::Numpad6)) airplanes[underControll]->addTrans(D3DXVECTOR3{ 0, 0, -10 });
		if (inputDevice->getTrigger(KeyCode::Numpad4)) airplanes[underControll]->addTrans(D3DXVECTOR3{ 0, 0, 10 });
		if (inputDevice->getTrigger(KeyCode::Space)) airplanes[underControll]->setTrans(D3DXVECTOR3{ 0, 0, 0 });

		if (inputDevice->getTrigger(KeyCode::Return)) state = State::Exit;
	}
	catch (const char* e)
	{
		textAreas.back()->text = e;
	}
}

void MainScene::update()
{
	for (const auto& gameObject : gameObjects) gameObject->update({ timer, inputDevice, gameObjects });

	auto colls = Collider::getCollisions({ airplanes[0]->getBBox() }, { airplanes[1]->getBBox() });

	textAreas.front()->text = "{\n    Airplane:\n    {\n";
	for (auto i = 0; i < 2; ++i)
	{
		auto pos = airplanes[i]->getBBox()->getPosition();
		textAreas.front()->text += (boost::format(
			"        { X: %2%, Y: %3%, Z: %4%, R: %5%, Hit: %6% }%7%\n"
		) % i % pos.x % pos.y % pos.z % airplanes[i]->getBBox()->getR() % (colls.size() > 0 ? "TRUE" : "FALSE") % (i == 1 ? "" : ",")).str();
	}
	textAreas.front()->text += (boost::format(
		"    },\n    Distance: %1%\n} \n%2%\n"
	) % Collider::calculateDistance(airplanes[0]->getBBox()->getPosition(), airplanes[1]->getBBox()->getPosition()) % timer->getSeconds()).str();

	textAreas.front()->text += (boost::format("\n%1%") % airplanes[0]->getUUID()).str();

	D3DXMATRIX mat;

	cameras.tps->update(airplanes[underControll]->getMat());
	cameras.fps->update(airplanes[underControll]->getMat());
	cameras.over->update();
}

void MainScene::render()
{
	graphics->SetCamera([&](const LPDIRECT3DDEVICE9 device) {
		if (camType == trau::CameraTypes::FPS) cameras.fps->set(device);
		else if (camType == trau::CameraTypes::TPS) cameras.tps->set(device);
		else if (camType == trau::CameraTypes::OVER) cameras.over->set(device);
	});

	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& gameObject : gameObjects) gameObject->draw(device);
	});
}
