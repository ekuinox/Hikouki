#pragma once

#ifndef ___ENEMY_AIRPLANE_H

#include "Airplane.h"
#include <random>

class EnemyAirplane : public Airplane {
public:
	struct Moves {
		float span;
		D3DXVECTOR3 trans, angle;
	};
	EnemyAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const char *);
	void update(const UpdateDetail&);
private:
	static constexpr auto rotationSpanSeconds = 1.0f;
	std::mt19937 randomEngine;
	std::unique_ptr<trau::Timer> rotationTimer;
	std::vector<Moves> moveTimeline;
	int moveTimelineIndex;
};

#define ___ENEMY_AIRPLANE_H
#endif