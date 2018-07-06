#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: width(_width), height(_height)
{
	// 入力初期化
	inputDevice = new Input(hinst, hwnd);

	graphics = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成

	if (!graphics->Init(hwnd, fullscreen, width, height)) {	// DirectX Graphicsオブジェクト初期化
		throw "DirectX 初期化エラー";
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