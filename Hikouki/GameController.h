#pragma once

#include <Windows.h>
#include <thread>
#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "GameObject.h"
#include "input.h"
#include "mathutil.h"
#include <vector>
#include <chrono>
#include "Airplain.h"

class GameController {
private:
	CDirectXGraphics * graphics;
	HANDLE event_handle;
	bool end;
	std::thread main_thread;

	using Camera = struct _Camera {
		D3DXVECTOR3 looking_at; // éãì_
		D3DXVECTOR3 looking_for; // íçéãì_
		D3DXVECTOR3 up;
		_Camera() :
			looking_at(D3DXVECTOR3(0.0, 0.0f, -80.0f)),
			looking_for(D3DXVECTOR3(0.0, 0.0f, 0.0f)),
			up(D3DXVECTOR3(0.0, 1.0f, 0.0f))
		{
		};
	};
	Camera *camera;
	std::vector<Airplain*> airplains;
	XFileObjectBase* skydome;
	bool tps;
	XFileManager *xfile_manager;
	int width, height;
	int under_controll;

public:
	GameController(HINSTANCE, HWND, int, int, bool);
	~GameController();
	void init(HINSTANCE, HWND, int, int, bool);
	void uninit();
	void main();
	void input();
	void update();
	void render();
	void setEvent();
	void setEndFlag();
};