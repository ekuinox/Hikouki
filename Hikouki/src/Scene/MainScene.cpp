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

	// �G��5�̐�������
	for (auto i = 0; i < enemiesCount; ++i)
		gameObjects.emplace_back(new EnemyAirplane(xFileManager->get("Airplane"), graphics->GetDXDevice(), "assets/GameObjectConfig/enemy.json"));

	// �v���C���𐶐����ăJ�����̃^�[�Q�b�g�ɃZ�b�g����
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

MainScene::Results MainScene::getResults()
{
	return results;
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

	// �v���C���̎����L���b�`
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

	// ���ɂ��Ă���
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
