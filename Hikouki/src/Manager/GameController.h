#pragma once

#ifndef ___GAME_CONTROLLER_H
#define ___GAME_CONTROLLER_H

#include <vector>
#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "EventMachine.h"
#include "../Utils/Input.h"
#include "../Scene/Scene.h"
#include "../SceneAttachments/Timer.h"

class GameController : public EventMachine {
private:
	CDirectXGraphics * graphics;
	Input *inputDevice;
	trau::Timer *timer;
	int currentSceneIndex;
	std::vector<std::unique_ptr<Scene>> scenes;
	XFileManager *xFileManager;
	int width, height;
public:
	GameController(HINSTANCE, HWND, int, int, bool);
	~GameController();
	void main();
};

#endif