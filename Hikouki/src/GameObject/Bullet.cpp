#include "Bullet.h"
#include "EnemyAirplane.h"
#include "../GameObjectAttachments/Collider.h"
#include "../Utils/MathUtil.h"

Bullet::Bullet(CDirect3DXFile * xfile, const LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(xfile), bbox(new BoundingSphere(xfile->GetMesh(), device)), state(State::PAUSE), timer(trau::Timer())
{
	trans.z = speed;
	hide();
	disable();
}

Bullet::~Bullet()
{
	delete bbox;
}

void Bullet::update(const UpdateDetail & detail)
{
	if (!active) return;

	if (state == State::TRIGGERED)
	{
		timer.end();
		if (timer.getSeconds() > lifeTimeSecond)
		{
			state = State::PAUSE;
			hide();
			disable();
		}
		else
		{
			// Õ“Ë‚ðŒŸõ
			for (const auto& gameObject : detail.gameObjects)
			{
				if (gameObject->getUUID() == getUUID()) break; // Ž©•ª‚ðœŠO

				if (gameObject->getId() == EnemyAirplane::id)
				{
					const auto& airplane = std::static_pointer_cast<EnemyAirplane>(gameObject);

					if (airplane->getState() == Airplane::State::ALIVE)
					{
						auto colls = Collider::getCollisions({ bbox }, { airplane->getBBox() });
						if (colls.size() > 0)
						{
							airplane->triggerExplosion();
							state = Bullet::State::HIT;
							hide();
							disable();
						}
					}
				}
			}
			mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
		}
	}
	
}

void Bullet::draw(const LPDIRECT3DDEVICE9 & device) const
{
	if (!drawing) return;

	if (state != State::TRIGGERED) return;

	XFileObjectBase::draw(device);
}

void Bullet::trigger(const D3DXMATRIX _mat)
{
	mat = _mat;
	state = State::TRIGGERED;
	timer.start();
	show();
	enable();
}

Bullet::State Bullet::getState()
{
	return state;
}
