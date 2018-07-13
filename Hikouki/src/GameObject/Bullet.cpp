#include "Bullet.h"
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
	// 初期化 OR コンテナに変更があった場合に
	if (detail.message & (GameObjectInterface::MESSAGE_INITIALIZE | GameObjectInterface::MESSAGE_ADDED | GameObjectInterface::MESSAGE_REMOVED))
	{
		// enemiesを設定する
		enemies.resize(0);

		for (const auto& gameObject : detail.gameObjects)
			if (gameObject->getId() == EnemyAirplane::id)
				enemies.emplace_back(std::static_pointer_cast<EnemyAirplane>(gameObject));
	}

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
			// 衝突を検索
			for (const auto& enemy : enemies)
			{
				if (enemy->getState() == Airplane::State::ALIVE)
				{
					auto colls = Collider::getCollisions({ bbox }, { enemy->getBBox() });
					if (colls.size() > 0)
					{
						enemy->triggerExplosion();
						state = Bullet::State::HIT;
						hide();
						disable();
					}
				}
			}
			mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
			bbox->updatePosition(mat);
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
