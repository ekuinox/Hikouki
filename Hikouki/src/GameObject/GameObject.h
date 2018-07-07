#pragma once

#ifndef ___GAME_OBJECT_H
#define ___GAME_OBJECT_H

#include <d3dx9.h>

class GameObjectInterface {
protected:
	bool drawing = false; // �`�悷�邩�̃t���O
	bool active = false; // �X�V���邩�̃t���O
	unsigned int layer = UINT_MAX; // �`��D��x
	unsigned int priority = UINT_MAX; // �X�V�����D��x
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