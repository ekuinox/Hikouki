#pragma once

#ifndef ___ENEMY_AIRPLANE_H

#include "Airplane.h"
#include <random>

class EnemyAirplane : public Airplane {
public:
	static constexpr unsigned int id = Airplane::id * 100 + 3;
	static constexpr auto rotationSpanSeconds = 1.0f;
	static constexpr auto initSpeed = 20.0f;
	struct Moves {
		float span;
		D3DXVECTOR3 trans, angle;
	};
	EnemyAirplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const char *);
	void update(const UpdateDetail&);
	virtual unsigned int getId() const { return id; }
private:
	std::mt19937 randomEngine;
	std::unique_ptr<trau::Timer> rotationTimer;
	std::vector<Moves> moveTimeline;
	int moveTimelineIndex;
};

#define ___ENEMY_AIRPLANE_H
#endif