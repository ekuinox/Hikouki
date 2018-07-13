#pragma once

#ifndef ___BULLET_H

#include "XFileObjectBase.h"
#include "../GameObjectAttachments/BoundingSphere.h"
#include "../SceneAttachments/Timer.h"
#include <memory>

class Bullet : public XFileObjectBase {
public:
	static constexpr unsigned int id = XFileObjectBase::id * 100 + 4;
	enum class State {
		PAUSE,
		TRIGGERED,
		HIT,
		EXIT,
	};
	Bullet(CDirect3DXFile * xfile, const LPDIRECT3DDEVICE9 device);
	~Bullet();
	void update(const UpdateDetail& detail);
	void draw(const LPDIRECT3DDEVICE9&) const;
	void trigger(const D3DXMATRIX _mat);
	State getState();
	virtual unsigned int getId() const { return id; }
private:
	static constexpr auto speed = 30.0f;
	static constexpr auto lifeTimeSecond = 5.0f; // ê∂ë∂éûä‘
	BoundingSphere * bbox;
	Bullet::State state;
	trau::Timer timer;
};

#define ___BULLET_H
#endif