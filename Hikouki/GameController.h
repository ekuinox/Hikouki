#pragma once

#ifndef ___GAME_CONTROLLER_H
#define ___GAME_CONTROLLER_H

#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "input.h"
#include "EventMachine.h"
#include "Timer.h"
#include <vector>
#include "Scene.h"
#include "MainScene.h"

class GameController : public EventMachine {
private:
	CDirectXGraphics * graphics;
	Input *inputDevice;
	trau::Timer *timer;
	std::unique_ptr<Scene> currentScene;
	XFileManager *xFileManager;
	int width, height;

public:
	GameController(HINSTANCE, HWND, int, int, bool);
	~GameController();
	void main();
};

#endif