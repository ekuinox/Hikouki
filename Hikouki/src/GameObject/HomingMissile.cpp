#include "HomingMissile.h"

HomingMissile::HomingMissile(CDirect3DXFile * _xfile, std::shared_ptr<XFileObjectBase> _target, const float & maxAngle, const D3DXVECTOR3& _position, const D3DXVECTOR3 & _velocity, trau::Timer * _timer)
	: XFileObjectBase(_xfile), target(_target), velocity(_velocity), addRotMax(D3DX_PI * maxAngle / 180.0f), position(_position), timer(_timer)
{
	D3DXQuaternionIdentity(&attitude);
	D3DXQuaternionRotationMatrix(&attitude, &mat);
}

void HomingMissile::update()
{
	auto targetVector = target->getPos() - position;
	auto zDir = velocity;

	D3DXVec3Normalize(&targetVector, &targetVector);
	D3DXVec3Normalize(&zDir, &zDir);

	double dot;
	auto targetAttitude = RotationArc(zDir, targetVector, dot);
	auto diffAngle = acos(dot);

	if (addRotMax >= diffAngle) attitude *= targetAttitude;
	else D3DXQuaternionSlerp(&attitude, &attitude, &(attitude * targetAttitude), addRotMax / diffAngle);

	D3DXMatrixRotationQuaternion(&mat, &attitude);

	velocity = D3DXVECTOR3(mat._31, mat._32, mat._33) * missileSpeed * timer->getSeconds();

	position += velocity;

	mat._41 = position.x;
	mat._42 = position.y;
	mat._43 = position.z;
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
