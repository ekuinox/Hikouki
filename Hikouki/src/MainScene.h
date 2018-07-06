#pragma once

#ifndef ___MAINSCENE_H

#include "Scene.h"

#include <memory>
#include <vector>
#include "Collider.h"
#include "GameObject.h"
#include "Airplain.h"
#include "Camera.h"
#include "TextArea.h"
#include <boost/format.hpp>

class MainScene : public Scene {
public:
	MainScene(CDirectXGraphics*, XFileManager*, Input*, trau::Timer*);
	Scene::State exec();
	Scene::State getState();
private:
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

	void input();
	void update();
	void render();
};

#define ___MAINSCENE_H
#endif