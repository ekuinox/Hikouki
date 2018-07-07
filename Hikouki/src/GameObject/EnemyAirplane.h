#pragma once

#ifndef ___ENEMY_AIRPLANE_H

#include "Airplane.h"
#include <random>

class EnemyAirplane : public Airplane {
private:
	static constexpr auto rotationSpanSeconds = 1.0f;
	std::mt19937 randomEngine;
	std::unique_ptr<trau::Timer> rotationTimer;
public:
	EnemyAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, trau::Timer*);
	EnemyAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, D3DXVECTOR3, trau::Timer*);
	void update();
};

#define ___ENEMY_AIRPLANE_H
#endif