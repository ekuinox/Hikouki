#include "PlayerAirplane.h"
#include "../Utils/MathUtil.h"
#include "../GameObjectAttachments/Collider.h"

PlayerAirplane::PlayerAirplane(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device, const D3DXVECTOR3 & coord)
	: Airplane(xfile, device, coord), enemy(nullptr)
{
	homingMissile = {
		std::unique_ptr<HomingMissile>(new HomingMissile(xfile, enemy, D3DX_PI * 30.0f / 180.0f, getPos(), D3DXVECTOR3{ 0, 0, 1 }, device)),
		false
	};

	homingMissile.first->hide();
	homingMissile.first->disable();

	trans.z = initSpeed;
	drawing_bbox = false;

	D3DXMatrixRotationY(&mat, D3DX_PI * 90.0f / 180.0f);
}

void PlayerAirplane::update(const UpdateDetail & detail)
{
	constexpr auto angleMax = 45.0f;
	constexpr auto addAngle = 1.0f;

	if (detail.input->getPress(KeyCode::A) && angle.y > -angleMax) angle.y -= addAngle;
	if (detail.input->getPress(KeyCode::D) && angle.y < angleMax) angle.y += addAngle;
	if (detail.input->getPress(KeyCode::S) && angle.x > -angleMax) angle.x -= addAngle;
	if (detail.input->getPress(KeyCode::W) && angle.x < angleMax) angle.x += addAngle;

	// î≠éÀ
	if (!homingMissile.second && detail.input->getTrigger(KeyCode::E)) triggerHomingMissile(detail.gameObjects);

	D3DXMATRIX mx;

	mathutils::makeWorldMatrix(mx, mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
	
	if (homingMissile.second) homingMissile.first->update(detail);

	// ÉqÉbÉgÇµÇƒÇ¢ÇΩÇ∆Ç´ÇÃèàóù
	if (homingMissile.first->getState() == HomingMissile::State::HIT)
	{
		homingMissile.second = false;
		homingMissile.first->pause();
	}
}

void PlayerAirplane::draw(const LPDIRECT3DDEVICE9 & device) const
{
	Airplane::draw(device);
	if (homingMissile.second) homingMissile.first->draw(device);
}

void PlayerAirplane::triggerHomingMissile(const std::vector<std::shared_ptr<GameObjectInterface>>& gameObjects)
{
	// àÍî‘ãﬂÇ¢ìGÇå©Ç¬ÇØèoÇ∑
	enemy = nullptr;
	float distance = 0.0f;

	for (const auto& gameObject : gameObjects)
	{
		if (typeid(*gameObject).hash_code() == typeid(EnemyAirplane).hash_code())
		{
			auto _enemy = std::static_pointer_cast<EnemyAirplane>(gameObject);
			const auto& _distance = Collider::calculateDistance(getPos(), _enemy->getPos());
			if (_enemy->getState() == Airplane::State::ALIVE && (enemy == nullptr || _distance < distance))
			{
				distance = _distance;
				enemy = _enemy;
			}
		}
	}

	if (enemy != nullptr)
	{
		homingMissile.second = true;
		homingMissile.first->trigger(enemy, getMat());
	}
}
