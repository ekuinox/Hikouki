#pragma once

#ifndef ___PLAYER_AIRPLANE_H

#include "EnemyAirplane.h"
#include "HomingMissile.h"
#include "Bullet.h"
#include <array>

class PlayerAirplane : public Airplane
{
public:
	static constexpr unsigned int id = Airplane::id * 100 + 1;
	PlayerAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const D3DXVECTOR3&, CDirect3DXFile*, CDirect3DXFile*);
	PlayerAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const D3DXVECTOR3&);
	void update(const UpdateDetail&);
	void draw(const LPDIRECT3DDEVICE9&) const;
	virtual unsigned int getId() const { return id; }

	std::vector<D3DXVECTOR3> getHomingMissilePositions();
private:
	std::shared_ptr<EnemyAirplane> enemy;
	std::unique_ptr<HomingMissile> homingMissile;
	std::array<std::unique_ptr<Bullet>, 100> bullets;
	void triggerHomingMissile(const std::vector<std::shared_ptr<GameObjectInterface>>&);
	void triggerBullet();
	void onOutside();
};

#define ___PLAYER_AIRPLANE_H
#endif
