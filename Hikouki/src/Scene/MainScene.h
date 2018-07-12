#pragma once

#ifndef ___MAINSCENE_H

#include "Scene.h"

#include <memory>
#include <vector>
#include <boost/format.hpp>
#include "../GameObject/GameObject.h"

class MainScene : public Scene {
public:
	MainScene(CDirectXGraphics*, XFileManager*, Input*, trau::Timer*);
	~MainScene();
	Scene::State exec();
	Scene::State getState();
private:
	std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;
	bool pausing;

	void sortGameObjectsLayer();
	void sortGameObjectsPriority();
	void input();
	void update();
	void render();
};

#define ___MAINSCENE_H
#endif