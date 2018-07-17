#include "GameController.h"
#include "../Scene/MainScene.h"
#include "../Scene/TitleScene.h"
#include "../Scene/ResultScene.h"

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

	sceneState = SceneState::Title;
	titleScene.reset(new TitleScene(graphics, xFileManager, inputDevice, timer));

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
		if (titleScene->exec() == Scene::State::Exit)
		{
			sceneState = SceneState::Main;
			mainScene.reset(new MainScene(graphics, xFileManager, inputDevice, timer));
			titleScene.reset();
		}
		break; 
	case SceneState::Main:
		if (mainScene->exec() == Scene::State::Exit)
		{
			sceneState = SceneState::Result;
			resultScene.reset(new ResultScene(graphics, xFileManager, inputDevice, timer));
			resultScene->setResults(mainScene->getResults());
			mainScene.reset();
		}
		break;
	case SceneState::Result:
		if (resultScene->exec() == Scene::State::Exit)
		{
			sceneState = SceneState::Title;
			titleScene.reset(new TitleScene(graphics, xFileManager, inputDevice, timer));
			resultScene.reset();
		}
		break;
	}
}
