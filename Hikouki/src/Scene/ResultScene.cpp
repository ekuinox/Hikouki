#include "ResultScene.h"
#include "../GameObject/Plain2D.h"

ResultScene::ResultScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer)
{
	LPDIRECT3DTEXTURE9 texture;
	if (FAILED(D3DXCreateTextureFromFile(graphics->GetDXDevice(), "assets/textures/kawaii-nadeshiko.png", &texture))) {
		throw std::runtime_error("テクスチャ読み込み失敗!");
	}

	gameObjects.emplace_back(new Plain2D({ graphics->GetWidth() / 2.0f, graphics->GetHeight() / 2.0f }, { 200, 200 }, { texture, 0.0f, 0.0f, 1.0f, 1.0f }));
}

ResultScene::~ResultScene()
{
}

Scene::State ResultScene::exec()
{
	timer->run([&] {
		input(); // 入力
		update(); // 更新
		render(); // 描画
	});
	return state;
}

Scene::State ResultScene::getState()
{
	return state;
}

void ResultScene::input()
{
	try
	{
		inputDevice->update();
		if (inputDevice->getTrigger(KeyCode::Return)) state = Scene::State::Exit;
	}
	catch (...)
	{
		return;
	}

}

void ResultScene::update()
{
	for (const auto& gameObject : gameObjects) gameObject->update({ timer, inputDevice, gameObjects });
	for (const auto& gameObject : gameObjects) gameObject->afterUpdate({ timer, inputDevice, gameObjects });
}

void ResultScene::render()
{
	for (const auto& gameObject : gameObjects) gameObject->beforeDraw(graphics->GetDXDevice());

	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& gameObject : gameObjects) gameObject->draw(device);
	});
}
