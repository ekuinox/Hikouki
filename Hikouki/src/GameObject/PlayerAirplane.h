#pragma once

#ifndef ___PLAYER_AIRPLANE_H

#include "EnemyAirplane.h"
#include "HomingMissile.h"

class PlayerAirplane : public Airplane
{
public:
	static constexpr auto initSpeed = 20.0f;
	enum class State {
		ALIVE,
		EXPLOSION
	};
	PlayerAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const D3DXVECTOR3&);
	void update(const UpdateDetail&);
	void draw(const LPDIRECT3DDEVICE9&) const;
private:
	std::shared_ptr<EnemyAirplane> enemy;
	std::unique_ptr<HomingMissile> homingMissile;
	void triggerHomingMissile(const std::vector<std::shared_ptr<GameObjectInterface>>&);
};

#define ___PLAYER_AIRPLANE_H
#endif
