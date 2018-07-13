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

	/* MESSAGE TYPES */
	static constexpr DWORD MESSAGE_NOTHING = 0b0000;
	static constexpr DWORD MESSAGE_INITIALIZE = 0b0001;
	static constexpr DWORD MESSAGE_FREEZE = 0b0010;
	static constexpr DWORD MESSAGE_ADDED = 0b0100;
	static constexpr DWORD MESSAGE_REMOVED = 0b1000;

	struct UpdateDetail {
		trau::Timer * timer;
		Input * input;
		const std::vector<std::shared_ptr<GameObjectInterface>>& gameObjects;
		DWORD message;
	};
	
	GameObjectInterface() : uuid(trau::utils::generateUUID()) {}
	virtual ~GameObjectInterface() {}
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
	virtual ~GameObject();
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
