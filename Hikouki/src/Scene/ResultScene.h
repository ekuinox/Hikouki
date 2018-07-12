#pragma once

#ifndef ___RESULT_SCENE_H

#include "Scene.h"

#include "Scene.h"
#include <vector>
#include <memory>
#include "../GameObject/GameObject.h"

class ResultScene : public Scene {
public:
	ResultScene(CDirectXGraphics*, XFileManager*, Input*, trau::Timer*);
	~ResultScene();
	Scene::State exec();
	Scene::State getState();
private:
	void input();
	void update();
	void render();

	std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;
};

#define ___RESULT_SCENE_H
#endif
