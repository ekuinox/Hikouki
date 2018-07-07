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
		if (rotationTimer->getSeconds() > rotationSpanSeconds)
		{
			rotationTimer->start();
			switch (randomEngine() % 3)
			{
			case 0:
				angle.x += (randomEngine() % 20) * randomEngine() % 2 == 0 ? -1 : 1;
				break;
			case 1:
				angle.y += (randomEngine() % 20) * randomEngine() % 2 == 0 ? -1 : 1;
				break;
			case 2:
				angle.z += (randomEngine() % 20) * randomEngine() % 2 == 0 ? -1 : 1;
				break;
			}
		}
		
		D3DXMATRIX mx;
		MakeWorldMatrix(mx, mat, angle * timer->getSeconds(), trans * timer->getSeconds());
		bbox->updatePosition(mat);
	}
}
