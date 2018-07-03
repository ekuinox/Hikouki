#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: width(_width), height(_height)
{
	// ���͏�����
	inputDevice = new Input(hinst, hwnd);

	graphics = new CDirectXGraphics();	// DirectX Graphics�I�u�W�F�N�g����

	if (!graphics->Init(hwnd, fullscreen, width, height)) {	// DirectX Graphics�I�u�W�F�N�g������
		throw "DirectX �������G���[";
	}

	timer = new trau::Timer();

	xFileManager = new XFileManager(graphics->GetDXDevice());

	currentScene = std::unique_ptr<MainScene>(new MainScene(graphics, xFileManager, inputDevice, timer));

	Start();
}

GameController::~GameController()
{
	Stop();

	if (graphics != nullptr) {
		currentScene.release();
		graphics->Exit();
		delete graphics;
	}

	delete inputDevice;
}

void GameController::main()
{
	currentScene->exec();
}