#pragma once

#ifndef ___MAINSCENE_H

#include "Scene.h"

#include <memory>
#include <vector>
#include <boost/format.hpp>
#include "../GameObject/GameObject.h"
#include "../GameObject/Airplane.h"
#include "../GameObject/TextArea.h"
#include "../SceneAttachments/Camera.h"

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

	trau::CameraTypes camType;

	std::vector<std::shared_ptr<Airplane>> airplanes;
	std::vector<std::shared_ptr<trau::TextArea>> textAreas;
	std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;

	int width, height;
	unsigned int underControll;

	void sortGameObjectsLayer();
	void sortGameObjectsPriority();
	void input();
	void update();
	void render();
};

#define ___MAINSCENE_H
#endif