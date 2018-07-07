#include "EnemyAirplane.h"
#include "../Utils/MathUtil.h"

EnemyAirplane::EnemyAirplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, trau::Timer *timer)
	: EnemyAirplane(_xfile, device, {}, timer)
{
}

EnemyAirplane::EnemyAirplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, D3DXVECTOR3 coord, trau::Timer *timer)
	: Airplane(_xfile, device, coord, timer)
{
	std::random_device seed_gen;
	std::mt19937 _engine(seed_gen());
	randomEngine = _engine;
	trans.z = 20.0f;
	rotationTimer = std::unique_ptr<trau::Timer>(new trau::Timer());
	moveTimeline = {
		Moves{ 1.0f, { 0, 0, 0 }, { 20, 0, 0 } },
		Moves{ 5.0f, { 0, 0, 0 }, { -20, 0, 0 } },
		Moves{ 3.0f, { 0, 0, 0 }, { 0, 30, 0 } },
		Moves{ 6.0f, { 0, 0, 0 }, { 5, 0, 0 } },
	};
	moveTimelineIndex = 0;
}

void EnemyAirplane::update()
{
	if (!active) return;

	if (explosion_flag)
	{
		explosion->Update();
	}
	else
	{
		rotationTimer->end();
		if (rotationTimer->getSeconds() > moveTimeline[moveTimelineIndex].span)
		{
			rotationTimer->start();
			trans += moveTimeline[moveTimelineIndex].trans;
			angle += moveTimeline[moveTimelineIndex].angle;
			
			moveTimelineIndex++;
			if (moveTimeline.size() == moveTimelineIndex) moveTimelineIndex = 0;
		}

		
		D3DXMATRIX mx;
		MakeWorldMatrix(mx, mat, angle * timer->getSeconds(), trans * timer->getSeconds());
		bbox->updatePosition(mat);
	}
}
