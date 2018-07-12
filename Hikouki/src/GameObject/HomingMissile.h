#pragma once

#ifndef ___HOMING_MISSILE_H

#include <memory>
#include "XFileObjectBase.h"
#include "../GameObjectAttachments/BoundingSphere.h"

class HomingMissile : public XFileObjectBase {
public:
	enum class State {
		PAUSE,
		FOLLOWING,
		HIT,
		EXIT,
	};
	static constexpr unsigned int id = XFileObjectBase::id * 100 + 2;
	static constexpr auto missileSpeed = 30.0f;
	HomingMissile(CDirect3DXFile* _xfile, std::shared_ptr<XFileObjectBase> _target, const float& maxAngle, const D3DXVECTOR3& _position,const D3DXVECTOR3& _velocity, LPDIRECT3DDEVICE9 device);
	void pause();
	void trigger(const std::shared_ptr<XFileObjectBase>& _target, const D3DXMATRIX& _owner_mat);
	void update(const UpdateDetail&);
	void draw(const LPDIRECT3DDEVICE9&) const;
	HomingMissile::State getState() const;
	virtual unsigned int getId() const { return HomingMissile::id; }
protected:
	D3DXQUATERNION RotationArc(D3DXVECTOR3 v0, D3DXVECTOR3 v1, double& d);
	std::shared_ptr<XFileObjectBase> target;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXQUATERNION attitude;
	float addRotMax;
	BoundingSphere * bbox;
	HomingMissile::State state;
};

#define ___HOMING_MISSILE_H
#endif
