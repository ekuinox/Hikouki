#pragma once

#ifndef ___TITLE_SCENE_H
#define ___TITLE_SCENE_H

#include "Scene.h"
#include "../GameObject/Plain2D.h"
#include <vector>
#include <memory>

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

	std::vector<std::shared_ptr<GameObject>> gameObjects;
};

#endif