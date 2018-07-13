#include "TitleScene.h"
#include "../GameObject/Plain2D.h"
#include "../GameObject/TextArea.h"

TitleScene::TitleScene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
	: Scene(_graphics, _xfileManager, _input, _timer)
{
	LPDIRECT3DTEXTURE9 texture;
	if (FAILED(D3DXCreateTextureFromFile(graphics->GetDXDevice(), "assets/textures/shion.png", &texture))) {
		throw std::runtime_error("�e�N�X�`���ǂݍ��ݎ��s!");
	}

	gameObjects.emplace_back(new Plain2D({ graphics->GetWidth() / 2.0f, graphics->GetHeight() / 2.0f }, { 200, 200 }, { texture, 0.0f, 0.0f, 1.0f, 1.0f }));

	gameObjects.emplace_back(new trau::TextArea(
		graphics->GetDXDevice(),
		100,
		100,
		"--- Enter�ŃX�^�[�g---\n"
		"���������߂�\n"
		"WASD�ŋ@�̑���\n"
		"Q�܂��͍��N���b�N�Œe�۔���\n"
		"E�܂��͉E�N���b�N�ŗU���~�T�C������"
		"V�Ŏ��_�؂�ւ�"
		"F5��BBox�\���؂�ւ�"
		"\n�[���������Ǝ��E���܂�\n\n\n"
		"�Y�㎇������񂩂킢���D�D�D�I�ǂ��т����ǂ�ł��������I�I"
	));
}

TitleScene::~TitleScene()
{
	gameObjects.clear();
}

Scene::State TitleScene::exec()
{
	timer->run([&] {
		input(); // ����
		update(); // �X�V
		render(); // �`��
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
