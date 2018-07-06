#include "TitleScene.h"

TitleScene::TitleScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer)
{
	LPDIRECT3DTEXTURE9 texture;
	if (FAILED(D3DXCreateTextureFromFile(graphics->GetDXDevice(), "assets/textures/shion.png", &texture))) {
		throw std::runtime_error("テクスチャ読み込み失敗!");
	}

	gameObjects.emplace_back(new Plain2D({ 400, 300 }, { 200, 200 }, Plain2D::TextureSets{ texture, 0.0f, 0.0f, 1.0f, 1.0f }));
}

TitleScene::~TitleScene()
{
	gameObjects.clear();
}

Scene::State TitleScene::exec()
{
	timer->run([&] {
		input(); // 入力
		update(); // 更新
		render(); // 描画
	});
	return state;
}

Scene::State TitleScene::getState()
{
	return state;
}

void TitleScene::input()
{
	try
	{
		inputDevice->update();
	}
	catch (const char* e)
	{
		return;
	}

	if (inputDevice->getPress(KeyCode::Return)) state = Scene::State::Exit;
}

void TitleScene::update()
{
}

void TitleScene::render()
{
	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& game_object : gameObjects) game_object->draw(device);
	});
}
