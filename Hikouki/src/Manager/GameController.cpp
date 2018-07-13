#include "GameController.h"
#include "../Scene/MainScene.h"
#include "../Scene/TitleScene.h"
#include "../Scene/ResultScene.h"

void GameController::initScenes()
{
	titleScene.reset();
	mainScene.reset();
	resultScene.reset();

	titleScene = std::unique_ptr<TitleScene>(new TitleScene(graphics, xFileManager, inputDevice, timer));
	mainScene = std::unique_ptr<MainScene>(new MainScene(graphics, xFileManager, inputDevice, timer));
	resultScene = std::unique_ptr<ResultScene>(new ResultScene(graphics, xFileManager, inputDevice, timer));

	sceneState = SceneState::Title;

	currentSceneIndex = 0;
}

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

	initScenes();

	Start();
}

GameController::~GameController()
{
	Stop();

	if (graphics != nullptr) {
		titleScene.reset();
		mainScene.reset();
		resultScene.reset();
		graphics->Exit();
		delete graphics;
		delete xFileManager;
	}

	delete inputDevice;
}

void GameController::main()
{
	switch (sceneState)
	{
	case SceneState::Title:
		if (titleScene->exec() == Scene::State::Exit) sceneState = SceneState::Main;
		break; 
	case SceneState::Main:
		if (mainScene->exec() == Scene::State::Exit)
		{
			sceneState = SceneState::Result;
			resultScene->setResults(mainScene->getResults());
		}
		break;
	case SceneState::Result:
		if (resultScene->exec() == Scene::State::Exit)
		{
			initScenes();
		}
		resultScene->exec();
		break;
	}
}
