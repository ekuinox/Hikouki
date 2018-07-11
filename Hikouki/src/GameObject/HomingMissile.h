#pragma once

#ifndef ___HOMING_MISSILE_H

#include <memory>
#include "XFileObjectBase.h"
#include "../GameObjectAttachments/BoundingSphere.h"

class HomingMissile : public XFileObjectBase {
public:
	static constexpr auto missileSpeed = 30.0f;
	HomingMissile(CDirect3DXFile* _xfile, std::shared_ptr<XFileObjectBase> _target, const float& maxAngle, const D3DXVECTOR3& _position,const D3DXVECTOR3& _velocity, LPDIRECT3DDEVICE9 device);
protected:
	void update(const UpdateDetail&);
	D3DXQUATERNION RotationArc(D3DXVECTOR3 v0, D3DXVECTOR3 v1, double& d);
	void draw(const LPDIRECT3DDEVICE9&) const;
	std::shared_ptr<XFileObjectBase> target;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXQUATERNION attitude;
	float addRotMax;
	BoundingSphere * bbox;
};

#define ___HOMING_MISSILE_H
#endif