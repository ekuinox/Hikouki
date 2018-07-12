#pragma once

#ifndef ___PLAYER_AIRPLANE_H

#include "EnemyAirplane.h"
#include "HomingMissile.h"

class PlayerAirplane : public Airplane
{
public:
	static constexpr unsigned int id = Airplane::id * 100 + 1;
	static constexpr auto initSpeed = 10.0f;
	enum class State {
		ALIVE,
		EXPLOSION
	};
	PlayerAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const D3DXVECTOR3&);
	void update(const UpdateDetail&);
	void draw(const LPDIRECT3DDEVICE9&) const;
	virtual unsigned int getId() const { return id; }

	std::vector<D3DXVECTOR3> getHomingMissilePositions();
private:
	std::shared_ptr<EnemyAirplane> enemy;
	std::unique_ptr<HomingMissile> homingMissile;
	void triggerHomingMissile(const std::vector<std::shared_ptr<GameObjectInterface>>&);
	void onOutside();
};

#define ___PLAYER_AIRPLANE_H
#endif
