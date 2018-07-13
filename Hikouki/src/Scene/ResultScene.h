#pragma once

#ifndef ___RESULT_SCENE_H

#include "Scene.h"

#include "Scene.h"
#include <vector>
#include <memory>
#include "MainScene.h"
#include "../GameObject/GameObject.h"

class ResultScene : public Scene {
public:
	ResultScene(CDirectXGraphics*, XFileManager*, Input*, trau::Timer*);
	~ResultScene();
	Scene::State exec();
	Scene::State getState();
	void setResults(const MainScene::Results& _results);
private:
	MainScene::Results results;
	void input();
	void update();
	void render();

	std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;
};

#define ___RESULT_SCENE_H
#endif
