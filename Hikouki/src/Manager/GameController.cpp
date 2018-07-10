#include "GameController.h"
#include "../Scene/MainScene.h"
#include "../Scene/TitleScene.h"

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

	scenes.emplace_back(std::unique_ptr<TitleScene>(new TitleScene(graphics, xFileManager, inputDevice, timer)));
	scenes.emplace_back(std::unique_ptr<MainScene>(new MainScene(graphics, xFileManager, inputDevice, timer)));

	currentSceneIndex = 0;

	Start();
}

GameController::~GameController()
{
	Stop();

	if (graphics != nullptr) {
		scenes.clear();
		graphics->Exit();
		delete graphics;
	}

	delete inputDevice;
}

void GameController::main()
{
	if (scenes[currentSceneIndex]->exec() == Scene::State::Exit) {
		currentSceneIndex++;
		if (currentSceneIndex == scenes.size()) {
			state = State::Exit;
		}
	}
}
