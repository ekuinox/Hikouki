#include "PlayerAirplane.h"
#include "../Utils/MathUtil.h"
#include "../GameObjectAttachments/Collider.h"

PlayerAirplane::PlayerAirplane(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device, const D3DXVECTOR3 & coord)
	: Airplane(xfile, device, coord), enemy(nullptr)
{
	homingMissile = std::unique_ptr<HomingMissile>(new HomingMissile(xfile, enemy, D3DX_PI * 10.0f / 180.0f, getPos(), D3DXVECTOR3{ 0, 0, 1 }, device));

	trans.z = initSpeed;

	// x軸方向に進めるように
	D3DXMatrixRotationY(&mat, D3DX_PI * 90.0f / 180.0f);
}

void PlayerAirplane::update(const UpdateDetail & detail)
{
	constexpr auto angleMax = 45.0f;
	constexpr auto addAngle = 1.0f;

	// 操作
	if (detail.input->getPress(KeyCode::A) && angle.y > -angleMax) angle.y -= addAngle;
	if (detail.input->getPress(KeyCode::D) && angle.y < angleMax) angle.y += addAngle;
	if (detail.input->getPress(KeyCode::S) && angle.x > -angleMax) angle.x -= addAngle;
	if (detail.input->getPress(KeyCode::W) && angle.x < angleMax) angle.x += addAngle;

	// BBoxを切り替える
	if (detail.input->getTrigger(KeyCode::F5)) drawingBBox = !drawingBBox;

	// 追尾ミサイルを発射する
	if (homingMissile->getState() == HomingMissile::State::PAUSE && detail.input->getTrigger(KeyCode::E))
		triggerHomingMissile(detail.gameObjects);

	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) homingMissile->update(detail);

	// ヒットしていたときの処理
	if (homingMissile->getState() == HomingMissile::State::HIT) homingMissile->pause();

	mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());

	bbox->updatePosition(mat);
}

void PlayerAirplane::draw(const LPDIRECT3DDEVICE9 & device) const
{
	Airplane::draw(device);
	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) homingMissile->draw(device);
}

const GameObjectInterface::Type PlayerAirplane::getType() const
{
	return GameObjectInterface::Type::PlayerAirplane;
}

void PlayerAirplane::triggerHomingMissile(const std::vector<std::shared_ptr<GameObjectInterface>>& gameObjects)
{
	// 一番近い敵を見つけ出す
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

	if (enemy != nullptr) homingMissile->trigger(enemy, getMat());
}
