#include "TitleScene.h"
#include "../GameObject/Plain2D.h"
#include "../GameObject/TextArea.h"

TitleScene::TitleScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer)
{
	LPDIRECT3DTEXTURE9 texture;
	if (FAILED(D3DXCreateTextureFromFile(graphics->GetDXDevice(), "assets/textures/shion.png", &texture))) {
		throw std::runtime_error("テクスチャ読み込み失敗!");
	}

	gameObjects.emplace_back(new Plain2D({ graphics->GetWidth() / 2.0f, graphics->GetHeight() / 2.0f }, { 200, 200 }, { texture, 0.0f, 0.0f, 1.0f, 1.0f }));

	gameObjects.emplace_back(new trau::TextArea(
		graphics->GetDXDevice(),
		100,
		100,
		"--- Enterでスタート---\n"
		"そうさせつめい\n"
		"WASDで機体操作\n"
		"Qまたは左クリックで弾丸発射\n"
		"Eまたは右クリックで誘導ミサイル発射"
		"Vで視点切り替え"
		"F5でBBox表示切り替え"
		"\n端っこいくと自殺します\n\n\n"
		"浦上紫苑ちゃんかわいい．．．！どうびじゅを読んでください！！"
	));
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
		if (inputDevice->getTrigger(KeyCode::Return)) state = Scene::State::Exit;
	}
	catch (...)
	{
		return;
	}
}

void TitleScene::update()
{
	for (const auto& gameObject : gameObjects) gameObject->update({ timer, inputDevice, gameObjects });
	for (const auto& gameObject : gameObjects) gameObject->afterUpdate({ timer, inputDevice, gameObjects });
}

void TitleScene::render()
{
	for (const auto& gameObject : gameObjects) gameObject->beforeDraw(graphics->GetDXDevice());

	graphics->Render([&](const LPDIRECT3DDEVICE9 device) {
		for (const auto& gameObject : gameObjects) gameObject->draw(device);
	});
}
