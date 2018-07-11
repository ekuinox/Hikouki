#pragma once

#ifndef ___PLAYER_AIRPLANE_H

#include "Airplane.h"

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
};

#define ___PLAYER_AIRPLANE_H
#endif
