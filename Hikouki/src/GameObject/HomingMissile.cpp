#include "HomingMissile.h"
#include "../GameObjectAttachments/Collider.h"

HomingMissile::HomingMissile(CDirect3DXFile * _xfile, std::shared_ptr<EnemyAirplane> _target, const float & maxAngle, const D3DXVECTOR3& _position, const D3DXVECTOR3 & _velocity, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(_xfile), target(_target), velocity(_velocity)
	, addRotMax(D3DX_PI * maxAngle / 180.0f), position(_position)
	, bbox(new BoundingSphere(xfile->GetMesh(), device))
{
	pause();
	D3DXQuaternionIdentity(&attitude);
	D3DXQuaternionRotationMatrix(&attitude, &mat);
}

HomingMissile::~HomingMissile()
{
	delete bbox;
}

void HomingMissile::pause()
{
	state = State::PAUSE;
	hide();
	disable();
}

void HomingMissile::trigger(const std::shared_ptr<EnemyAirplane>& _target, const D3DXMATRIX & _owner_mat)
{
	target = _target;

	D3DXQuaternionRotationMatrix(&attitude, &_owner_mat);

	position = D3DXVECTOR3(
		_owner_mat._41,
		_owner_mat._42,
		_owner_mat._43
	);
	
	state = State::FOLLOWING;

	enable();
	show();
}

void HomingMissile::update(const UpdateDetail& detail)
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

	if (state != State::FOLLOWING) return;

	auto targetVector = target->getPos() - position;
	auto zDir = velocity;

	D3DXVec3Normalize(&targetVector, &targetVector);
	D3DXVec3Normalize(&zDir, &zDir);

	double dot;
	auto targetAttitude = RotationArc(zDir, targetVector, dot);
	auto diffAngle = acosf(static_cast<float>(dot));

	if (addRotMax >= diffAngle) attitude *= targetAttitude;
	else D3DXQuaternionSlerp(&attitude, &attitude, &(attitude * targetAttitude), addRotMax / diffAngle);

	D3DXMatrixRotationQuaternion(&mat, &attitude);

	velocity = D3DXVECTOR3(mat._31, mat._32, mat._33) * missileSpeed * detail.timer->getSeconds();

	position += velocity;

	mat._41 = position.x;
	mat._42 = position.y;
	mat._43 = position.z;

	bbox->updatePosition(mat);

	// 敵がすでくたばっていたら再設定をする
	if (target->getState() != Airplane::State::ALIVE)
	{
		std::shared_ptr<EnemyAirplane> enemy = nullptr;
		float distance = 0.0f;

		for (const auto& _enemy : enemies)
		{
			const auto& _distance = Collider::calculateDistance(getPos(), _enemy->getPos());
			if (_enemy->getState() == Airplane::State::ALIVE && (enemy == nullptr || _distance < distance))
			{
				distance = _distance;
				enemy = _enemy;
			}
		}

		// 居たらそいつに向けてトリガしなおす
		if (enemy != nullptr) trigger(enemy, getMat());
		else pause();
	}

	// あたりを見る
	for (const auto& enemy : enemies)
	{
		if (enemy->getState() == Airplane::State::ALIVE)
		{
			const auto& colls = Collider::getCollisions({ bbox }, { enemy->getBBox() });
			if (colls.size() > 0)
			{
				enemy->triggerExplosion();
				state = HomingMissile::State::HIT;
				hide();
				disable();
			}
		}
	}
}

void HomingMissile::draw(const LPDIRECT3DDEVICE9 & device) const
{
	if (!drawing) return;
	XFileObjectBase::draw(device);
}

HomingMissile::State HomingMissile::getState() const
{
	return state;
}

D3DXQUATERNION HomingMissile::RotationArc(D3DXVECTOR3 v0, D3DXVECTOR3 v1, double & d)
{
	D3DXVECTOR3 axis;

	D3DXVec3Cross(&axis, &v0, &v1);

	d = D3DXVec3Dot(&v0, &v1);
	if (d > 1.0) d = 1.0;
	if (d < -1.0) d = -1.0;

	const auto& s = static_cast<float>(sqrt((1 + d) * 2));

	return D3DXQUATERNION(
		axis.x / s,
		axis.y / s,
		axis.z / s,
		s / 2
	);
}
