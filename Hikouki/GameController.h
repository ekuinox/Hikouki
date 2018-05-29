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

#ifdef _DEBUG
#include "DebugConsole.h"
#include "CDebugFont.h"
#endif
enum class CameraTypes : char {
	FPS = 0,
	TPS = 1,
	OVER = 2 // ÇÆÇÈÇÆÇÈÉJÉÅÉâ
};

constexpr CameraTypes operator++(CameraTypes& c, int)
{
	auto current = c;

	switch (c)
	{
	case CameraTypes::FPS:
		c = CameraTypes::TPS;
		break;
	case CameraTypes::TPS:
		c = CameraTypes::OVER;
		break;
	case CameraTypes::OVER:
		c = CameraTypes::FPS;
		break;
	}
	return current;
}

class GameController {
private:
	CDirectXGraphics * graphics;
	HANDLE event_handle;
	bool end;
	std::thread main_thread;

	using Camera = struct _Camera {
		D3DXVECTOR3 eye; // éãì_
		D3DXVECTOR3 look_at; // íçéãì_
		D3DXVECTOR3 up;
		_Camera() :
			eye(D3DXVECTOR3(0.0, 0.0f, -80.0f)),
			look_at(D3DXVECTOR3(0.0, 0.0f, 0.0f)),
			up(D3DXVECTOR3(0.0, 1.0f, 0.0f))
		{
		};
	};
	Camera *camera;
	std::vector<Airplain*> airplains;
	XFileObjectBase* skydome;
	XFileManager *xfile_manager;
	int width, height;
	int under_controll;
	struct {
		float azimuth; // ï˚à äp
		float elevation; // ã¬äp
		float distance; // ãóó£
	} over_camera;

	CameraTypes view_type;
	D3DXMATRIX view;

	Input *input_device;
	Keyboard *keyboard;
	Mouse *mouse;
#ifdef _DEBUG
	CDebugFont *debug_font;
	std::string debug_text;
#endif // _DEBUG


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