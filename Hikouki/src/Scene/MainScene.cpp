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
		{ "Airplane", "assets/f1.x" }, // ��s�@
		{ "Skydome", "assets/skydome.x" } // �X�J�C�h�[��
	});

	gameObjects.emplace_back(new Skydome(xFileManager->get("Skydome"), graphics->GetDXDevice()));

	// �G��5�̐�������
	for (auto i = 0; i < 5; ++i)
		gameObjects.emplace_back(new EnemyAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), "assets/GameObjectConfig/enemy.json"));

	// �v���C���𐶐����ăJ�����̃^�[�Q�b�g�ɃZ�b�g����
	const auto& cameraTarget = std::shared_ptr<XFileObjectBase>(new PlayerAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), D3DXVECTOR3{ 0, 0, 0 }));
	gameObjects.emplace_back(cameraTarget);
	gameObjects.emplace_back(new Camera(cameraTarget, graphics->GetWidth(), graphics->GetHeight()));

	// �����ݒ�
	graphics->SetRenderStateArray({
		{ D3DRS_ZENABLE, TRUE }, // �y�o�b�t�@�L��
		{ D3DRS_LIGHTING, true }, // ���C�g�L��
		{ D3DRS_CULLMODE, D3DCULL_NONE }, // �J�����O������
		{ D3DRS_AMBIENT, 0xffffffff }, // �����Z�b�g
		{ D3DRS_ALPHABLENDENABLE, TRUE }, // �A���t�@�E�u�����f�B���O���s��
		{ D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA }, // ���ߏ������s��
		{ D3DRS_SRCBLEND, D3DBLEND_SRCALPHA } // �������������s��
	});
}

Scene::State MainScene::exec()
{
	timer->run([&] {
		sortGameObjectsPriority();
		input(); // ����
		update(); // �X�V
		sortGameObjectsLayer();
		render(); // �`��
	});
	return state;
}

Scene::State MainScene::getState()
{
	return state;
}

void MainScene::sortGameObjectsLayer()
{
	// �`�揇�Ȃ̂ō~���ɂ���
	std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObjectInterface>& a, const std::shared_ptr<GameObjectInterface>& b) {
		return a->getLayer() > b->getLayer();
	});
}

void MainScene::sortGameObjectsPriority()
{
	// update�������Ȃ̂ŏ����ɂ���
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
