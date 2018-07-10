#pragma once

#ifndef ___HOMING_MISSILE_H

#include <memory>
#include "XFileObjectBase.h"
#include "../SceneAttachments//Timer.h"

class HomingMissile : public XFileObjectBase {
public:
	static constexpr auto missileSpeed = 20.0f;
	HomingMissile(CDirect3DXFile* _xfile, std::shared_ptr<XFileObjectBase> _target, const float& maxAngle, const D3DXVECTOR3& _position,const D3DXVECTOR3& _velocity, trau::Timer * _timer);
protected:
	void update(const UpdateDetail&);
	D3DXQUATERNION RotationArc(D3DXVECTOR3 v0, D3DXVECTOR3 v1, double& d);
	std::shared_ptr<XFileObjectBase> target;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXQUATERNION attitude;
	float addRotMax;
	trau::Timer * timer;
};

#define ___HOMING_MISSILE_H
#endif