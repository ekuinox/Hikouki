#pragma once

#ifndef ___GAME_OBJECT_H
#define ___GAME_OBJECT_H

#include <vector>
#include <memory>
#include <typeinfo>
#include <d3dx9.h>
#include "../Utils/Input.h"
#include "../Utils/Utils.h"
#include "../SceneAttachments/Timer.h"

class GameObjectInterface {
private:
	std::string uuid;
protected:
	bool drawing = false; // 描画するかのフラグ
	bool active = false; // 更新するかのフラグ
	unsigned int layer = UINT_MAX; // 描画優先度
	unsigned int priority = UINT_MAX; // 更新処理優先度
public:
	static constexpr unsigned int id = 0; // 識別用id
	struct UpdateDetail {
		trau::Timer * timer;
		Input * input;
		std::vector<std::shared_ptr<GameObjectInterface>> gameObjects;
	};
	GameObjectInterface() : uuid(trau::utils::generateUUID()) {}
	virtual void beforeDraw(const LPDIRECT3DDEVICE9&) = 0;
	virtual void draw(const LPDIRECT3DDEVICE9&) const = 0;
	virtual void update(const UpdateDetail&) = 0;
	virtual void afterUpdate(const UpdateDetail&) = 0;
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;
	virtual unsigned int getLayer() const = 0;
	virtual unsigned int getPriority() const = 0;
	virtual unsigned int getId() const { return id; }
	std::string getUUID() const { return uuid; }
};

class GameObject : public GameObjectInterface
{
public:
	static constexpr unsigned int id = GameObjectInterface::id + 1;
	GameObject();
	virtual void beforeDraw(const LPDIRECT3DDEVICE9&);
	virtual void draw(const LPDIRECT3DDEVICE9&) const;
	virtual void update(const UpdateDetail&);
	virtual void afterUpdate(const UpdateDetail&);
	virtual void show();
	virtual void hide();
	virtual void enable();
	virtual void disable();
	virtual unsigned int getLayer() const;
	virtual unsigned int getPriority() const;
	virtual unsigned int getId() const { return id; }
};

#endif
