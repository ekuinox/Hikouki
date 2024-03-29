#pragma once

#ifndef ___HOMING_MISSILE_H

#include <memory>
#include "XFileObjectBase.h"
#include "EnemyAirplane.h"
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
	static constexpr auto missileSpeed = 70.0f;
	HomingMissile(CDirect3DXFile* _xfile, std::shared_ptr<EnemyAirplane> _target, const float& maxAngle, const D3DXVECTOR3& _position,const D3DXVECTOR3& _velocity, LPDIRECT3DDEVICE9 device);
	~HomingMissile();
	void pause();
	void trigger(const std::shared_ptr<EnemyAirplane>& _target, const D3DXMATRIX& _owner_mat);
	void update(const UpdateDetail&);
	void draw(const LPDIRECT3DDEVICE9&) const;
	HomingMissile::State getState() const;
	virtual unsigned int getId() const { return HomingMissile::id; }
protected:
	D3DXQUATERNION RotationArc(D3DXVECTOR3 v0, D3DXVECTOR3 v1, double& d);
	std::shared_ptr<EnemyAirplane> target;
	std::vector<std::shared_ptr<EnemyAirplane>> enemies;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXQUATERNION attitude;
	float addRotMax;
	BoundingSphere * bbox;
	HomingMissile::State state;
};

#define ___HOMING_MISSILE_H
#endif
