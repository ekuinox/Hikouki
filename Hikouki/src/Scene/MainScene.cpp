#include "MainScene.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include "../GameObject/EnemyAirplane.h"
#include "../GameObject/HomingMissile.h"
#include "../GameObjectAttachments/Collider.h"
#include "../GameObject/PlayerAirplane.h"
#include "../GameObject/Camera.h"
#include "../GameObject/Skydome.h"

MainScene::MainScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer)
{
	xFileManager = new XFileManager(graphics->GetDXDevice());
	xFileManager->add({
		{ "Airplane", "assets/f1.x" }, // 飛行機
		{ "Skydome", "assets/skydome.x" } // スカイドーム
	});

	gameObjects.emplace_back(new Skydome(xFileManager->get("Skydome"), graphics->GetDXDevice()));

	// 敵を5体生成する
	for (auto i = 0; i < 5; ++i)
		gameObjects.emplace_back(new EnemyAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), "assets/GameObjectConfig/enemy.json"));

	// プレイヤを生成してカメラのターゲットにセットする
	const auto& cameraTarget = std::shared_ptr<XFileObjectBase>(new PlayerAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), D3DXVECTOR3{ 0, 0, 0 }));
	gameObjects.emplace_back(cameraTarget);
	gameObjects.emplace_back(new Camera(cameraTarget, graphics->GetWidth(), graphics->GetHeight()));

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

		if (inputDevice->getTrigger(KeyCode::Return)) state = State::Exit;
	}
	catch (...)
	{
	}
}

void MainScene::update()
{
	for (const auto& gameObject : gameObjects) gameObject->update({ timer, inputDevice, gameObjects });

	for (const auto& gameObject : gameObjects) gameObject->afterUpdate({ timer, inputDevice, gameObjects });
}

void MainScene::render()
{
	for (const auto& gameObject : gameObjects) gameObject->beforeDraw(graphics->GetDXDevice());
	
	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& gameObject : gameObjects) gameObject->draw(device);
	});
}
