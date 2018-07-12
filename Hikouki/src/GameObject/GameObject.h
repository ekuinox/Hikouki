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
	bool drawing = false; // �`�悷�邩�̃t���O
	bool active = false; // �X�V���邩�̃t���O
	unsigned int layer = UINT_MAX; // �`��D��x
	unsigned int priority = UINT_MAX; // �X�V�����D��x
public:
	// GameObjectInterface���p�������N���X���m�F���邽�߂�enum�D�N���X��ǉ����邽�тɂ����ɒǉ�����K�v������D�D�D
	enum class Type {
		GameObjectInterface,
		GameObject,
		Explosion,
		Plain2D,
		TextArea,
		XFileObjectBase,
		Airplane,
		EnemyAirplane,
		PlayerAirplane,
		HomingMissile,
		Camera,
		Skydome
	};
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
	virtual const GameObjectInterface::Type getType() const { return Type::GameObjectInterface; }
	virtual unsigned int getId() { return 0; }
	static unsigned int getIdStatic() { return 0; }
	std::string getUUID() const { return uuid; }
};

class GameObject : public GameObjectInterface
{
public:
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
	virtual const GameObjectInterface::Type getType() const;
};

#endif
