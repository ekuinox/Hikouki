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