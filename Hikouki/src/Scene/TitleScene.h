#pragma once

#ifndef ___TITLE_SCENE_H
#define ___TITLE_SCENE_H

#include "Scene.h"
#include <vector>
#include <memory>
#include "../GameObject/GameObject.h"

class TitleScene : public Scene {
public:
	TitleScene(CDirectXGraphics*, XFileManager*, Input*, trau::Timer*);
	~TitleScene();
	Scene::State exec();
	Scene::State getState();
private:
	void input();
	void update();
	void render();

	std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;
};

#endif