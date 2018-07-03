#pragma once

#ifndef ___SCENE_H

#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "input.h"
#include "Timer.h"

class Scene {
public:
	enum class State {
		Ready, Exit
	};
	Scene(CDirectXGraphics* _graphics, XFileManager *_xfileManager, Input* _input, trau::Timer* _timer)
		: graphics(_graphics), xFileManager(_xfileManager), inputDevice(_input), timer(_timer) {}
	virtual State exec() = 0;
	virtual State getState() = 0;
protected:
	CDirectXGraphics *graphics;
	Input *inputDevice;
	trau::Timer *timer;
	XFileManager *xFileManager;
	State state;

	virtual void input() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

};

#define ___SCENE_H
#endif
