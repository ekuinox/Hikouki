#pragma once

#ifndef ___GAME_CONTROLLER_H
#define ___GAME_CONTROLLER_H

#include <vector>
#include <unordered_map>
#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "EventMachine.h"
#include "../Utils/Input.h"
#include "../Scene/Scene.h"
#include "../SceneAttachments/Timer.h"
#include "../Scene/MainScene.h"
#include "../Scene/TitleScene.h"
#include "../Scene/ResultScene.h"

class GameController : public EventMachine {
private:
	enum class SceneState {
		Main,
		Title,
		Result
	};
	CDirectXGraphics * graphics;
	Input *inputDevice;
	trau::Timer *timer;
	int currentSceneIndex;
	SceneState sceneState;
	std::unique_ptr<MainScene> mainScene;
	std::unique_ptr<TitleScene> titleScene;
	std::unique_ptr<ResultScene> resultScene;
	XFileManager *xFileManager;
	int width, height;

	void initScenes();
public:
	GameController(HINSTANCE, HWND, int, int, bool);
	~GameController();
	void main();
};

#endif