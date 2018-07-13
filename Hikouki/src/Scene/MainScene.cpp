#include "MainScene.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include "../GameObject/EnemyAirplane.h"
#include "../GameObject/HomingMissile.h"
#include "../GameObjectAttachments/Collider.h"
#include "../GameObject/Camera.h"
#include "../GameObject/Skydome.h"
#include "../GameObject/Rader.h"

MainScene::MainScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer), pausing(false)
{
	xFileManager = new XFileManager(graphics->GetDXDevice());
	xFileManager->add({
		{ "Airplane", "assets/f1.x" },
		{ "Skydome", "assets/skydome.x" },
		{ "Bullet", "assets/bullet.x" },
		{ "Missile", "assets/missile.x" }
	});

	gameObjects.emplace_back(new Skydome(xFileManager->get("Skydome"), graphics->GetDXDevice()));

	static constexpr auto enemiesCount = 15;

	// 敵を5体生成する
	for (auto i = 0; i < enemiesCount; ++i)
		gameObjects.emplace_back(new EnemyAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), "assets/GameObjectConfig/enemy.json"));

	// プレイヤを生成してカメラのターゲットにセットする
	player = std::shared_ptr<PlayerAirplane>(new PlayerAirplane(
		xFileManager->get("Airplane"),
		graphics->GetDXDevice(),
		D3DXVECTOR3{ 0, 0, 0 },
		xFileManager->get("Missile"),
		xFileManager->get("Bullet")
	));
	gameObjects.emplace_back(player);
	gameObjects.emplace_back(new Camera(player, graphics->GetWidth(), graphics->GetHeight()));
	gameObjects.emplace_back(new Rader(player, { 100, 100 }, 80));

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

	state = State::Ready;
	updateMessage = GameObjectInterface::MESSAGE_INITIALIZE;
}

MainScene::~MainScene()
{
	gameObjects.clear();
}

Scene::State MainScene::exec()
{
	state = State::Exec;
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

MainScene::Results MainScene::getResults()
{
	return results;
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

		if (inputDevice->getTrigger(KeyCode::F3))
		{
			if (pausing) for (const auto& gameObject : gameObjects) gameObject->enable();
			else for (const auto& gameObject : gameObjects) gameObject->disable();
			pausing = !pausing;
		}

		if (inputDevice->getTrigger(KeyCode::Return)) exit();
	}
	catch (...)
	{
	}
}

void MainScene::update()
{
	for (const auto& gameObject : gameObjects) gameObject->update({ timer, inputDevice, gameObjects, updateMessage });

	for (const auto& gameObject : gameObjects) gameObject->afterUpdate({ timer, inputDevice, gameObjects, updateMessage });

	// プレイヤの死をキャッチ
	if (player->getState() == Airplane::State::EXIT) exit();

	bool isAllEnemiesDead = true;
	for (const auto& gameObject : gameObjects)
	{
		if (gameObject->getId() == EnemyAirplane::id && std::static_pointer_cast<EnemyAirplane>(gameObject)->getState() != Airplane::State::EXIT)
		{
			isAllEnemiesDead = false;
			break;
		}
	}

	if (isAllEnemiesDead)
	{
		exit();
	}

	// 無にしておく
	updateMessage = GameObjectInterface::MESSAGE_NOTHING;
}

void MainScene::render()
{
	for (const auto& gameObject : gameObjects) gameObject->beforeDraw(graphics->GetDXDevice());
	
	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& gameObject : gameObjects) gameObject->draw(device);
	});
}

void MainScene::exit()
{
	results = { 0, 0 };

	for (const auto& gameObject : gameObjects)
	{
		if (gameObject->getId() == EnemyAirplane::id)
		{
			++results.enemiesCount;
			if (std::static_pointer_cast<EnemyAirplane>(gameObject)->getState() != EnemyAirplane::State::ALIVE)
				++results.defeatEnemiesCount;
		}
	}

	state = State::Exit;

}
