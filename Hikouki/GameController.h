#pragma once

#include "CDirectXGraphics.h"
#include "XFileManager.h"
#include "GameObject.h"
#include "input.h"
#include "mathutil.h"
#include <vector>
#include <chrono>
#include "Airplain.h"
#include "Collider.h"
#include "EventMachine.h"
#include "Timer.h"
#include "TextArea.h"
#include <boost/format.hpp>

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

class GameController : public EventMachine {
private:
	CDirectXGraphics * graphics;
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
	trau::TextArea *text_area;
	XFileManager *xfile_manager;
	int width, height;
	int under_controll;
	struct {
		float azimuth; // ï˚à äp
		float elevation; // ã¬äp
		float distance; // ãóó£
	} over_camera;

	CameraTypes view_type;
	D3DXMATRIX view, proj;

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