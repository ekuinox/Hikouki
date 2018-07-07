#pragma once

#ifndef ___GAME_OBJECT_H
#define ___GAME_OBJECT_H

#include <d3dx9.h>

class GameObjectInterface {
protected:
	bool drawing = false; // 描画するかのフラグ
	bool active = false; // 更新するかのフラグ
	unsigned int layer = UINT_MAX; // 描画優先度
	unsigned int priority = UINT_MAX; // 更新処理優先度
public:
	virtual void draw(const LPDIRECT3DDEVICE9&) const = 0;
	virtual void update() = 0;
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;
	virtual unsigned int getLayer() = 0;
	virtual unsigned int getPriority() = 0;
};

class GameObject : public GameObjectInterface
{
public:
	virtual void draw(const LPDIRECT3DDEVICE9&) const;
	virtual void update();
	virtual void show();
	virtual void hide();
	virtual void enable();
	virtual void disable();
	virtual unsigned int getLayer();
	virtual unsigned int getPriority();
};

#endif