#pragma once

#ifndef ___MAINSCENE_H

#include "Scene.h"

#include <memory>
#include <vector>
#include "../GameObject/GameObject.h"
#include "../GameObject/PlayerAirplane.h"

class MainScene : public Scene {
public:
	struct Results {
		int defeatEnemiesCount;
		int enemiesCount;
	};
	MainScene(CDirectXGraphics*, XFileManager*, Input*, trau::Timer*);
	~MainScene();
	Scene::State exec();
	Scene::State getState();
	Results getResults();
private:
	std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;
	std::shared_ptr<PlayerAirplane> player;
	bool pausing;
	Results results;
	DWORD updateMessage;

	void sortGameObjectsLayer();
	void sortGameObjectsPriority();
	void input();
	void update();
	void render();
	void exit();
};

#define ___MAINSCENE_H
#endif