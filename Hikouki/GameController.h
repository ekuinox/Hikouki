#pragma once

#ifndef ___GAME_CONTROLLER_H
#define ___GAME_CONTROLLER_H

#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "GameObject.h"
#include "input.h"
#include "mathutil.h"
#include "Airplain.h"
#include "Collider.h"
#include "EventMachine.h"
#include "Timer.h"
#include "TextArea.h"
#include "Camera.h"
#include <vector>
#include <chrono>
#include <memory>
#include <tuple>
#include <boost/format.hpp>

class GameController : public EventMachine {
private:
	CDirectXGraphics * graphics;
	
	struct {
		std::unique_ptr<trau::TPSCamera> tps;
		std::unique_ptr<trau::FPSCamera> fps;
		std::unique_ptr<trau::OverCamera> over;
	} cameras;

	trau::CameraTypes cam_types;

	std::vector<std::shared_ptr<Airplain>> airplains;
	std::vector<std::shared_ptr<trau::TextArea>> text_areas;
	std::vector<std::shared_ptr<GameObject>> game_objects;

	XFileManager *xfile_manager;
	int width, height;
	int under_controll;

	Input *input_device;

	trau::Timer *timer;

public:
	GameController(HINSTANCE, HWND, int, int, bool);
	~GameController();
	void init(HINSTANCE, HWND, int, int, bool);
	void uninit();
	void main();
	void input();
	void update();
	void render();
};

#endif