#include "Collider.h"

float Collider::calculateDistance(const D3DXVECTOR3 & a, const D3DXVECTOR3 & b)
{
	return sqrt(
		fabs(pow(a.x - b.x, 2))
		+ fabs(pow(a.y - b.y, 2))
		+ fabs(pow(a.z - b.z, 2))
	);
}

bool Collider::isCollision(BoundingSphere * a, BoundingSphere * b)
{
	// ”¼Œa‚Ì˜a‚Æ’†S“¯Žm‚Ì‚Ì‹——£‚ð”äŠr‚·‚é
	return calculateDistance(a->getPosition(), b->getPosition()) < (a->getHitData().r + b->getHitData().r);
}

std::vector<std::pair<BoundingSphere*, BoundingSphere*>> Collider::getCollisions(std::vector<BoundingSphere*> a, std::vector<BoundingSphere*> b)
{
	std::vector<std::pair<BoundingSphere*, BoundingSphere*>> result;
	for (const auto& _a : a)
	{
		for (const auto& _b : b)
		{
			if (isCollision(_a, _b))
			{
				result.push_back({ _a, _b });
			}
		}
	}
	return result;
}
