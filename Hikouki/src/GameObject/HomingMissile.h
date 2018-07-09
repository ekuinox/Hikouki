#pragma once

#ifndef ___HOMING_MISSILE_H

#include "XFileObjectBase.h"

class HomingMissile : public XFileObjectBase {
public:
	HomingMissile(CDirect3DXFile* _xfile, XFileObjectBase* _target, const float& maxAngle, const D3DXVECTOR3& _velocity);
	void update();
protected:
	const D3DXQUATERNION& RotationArc(D3DXVECTOR3 v0, D3DXVECTOR3 v1, double& d);
	XFileObjectBase * target;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXQUATERNION attitude;
	float addRotMax;
};

#define ___HOMING_MISSILE_H
#endif