#include "PlayerAirplane.h"
#include "../Utils/MathUtil.h"
#include "../GameObjectAttachments/Collider.h"
#include "../GameObject/Skydome.h"

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
	if (!active) return;

	constexpr auto angleMax = 45.0f;
	constexpr auto addAngle = 10.0f;

	// ゼロに戻す
	angle = D3DXVECTOR3(0, 0, 0);

	// 操作
	if (detail.input->getPress(KeyCode::A) && angle.y > -angleMax) angle.y -= addAngle;
	if (detail.input->getPress(KeyCode::D) && angle.y < angleMax) angle.y += addAngle;
	if (detail.input->getPress(KeyCode::W) && angle.x > -angleMax) angle.x -= addAngle;
	if (detail.input->getPress(KeyCode::S) && angle.x < angleMax) angle.x += addAngle;

	// BBoxを切り替える
	if (detail.input->getTrigger(KeyCode::F5)) drawingBBox = !drawingBBox;

	// 追尾ミサイルを発射する
	if (homingMissile->getState() == HomingMissile::State::PAUSE && detail.input->getTrigger(KeyCode::E))
		triggerHomingMissile(detail.gameObjects);

	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) homingMissile->update(detail);

	// ヒットしていたときの処理
	if (homingMissile->getState() == HomingMissile::State::HIT) homingMissile->pause();

	// スカイドームの外
	for (const auto& gameObject : detail.gameObjects)
	{
		if (gameObject->getId() == Skydome::id)
		{
			const auto& r = std::static_pointer_cast<Skydome>(gameObject)->getBBox().get()->getR();
			
			// 画面端の判定
			if (fabs(getPos().x) > r || fabs(getPos().y) > r || fabs(getPos().z > r))
			{
				onOutside();
			}
		}
	}

	mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());

	bbox->updatePosition(mat);
}

void PlayerAirplane::draw(const LPDIRECT3DDEVICE9 & device) const
{
	Airplane::draw(device);
	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) homingMissile->draw(device);
}

std::vector<D3DXVECTOR3> PlayerAirplane::getHomingMissilePositions()
{
	std::vector<D3DXVECTOR3> result = {};

	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) result.emplace_back(homingMissile->getPos());

	return result;
}

void PlayerAirplane::triggerHomingMissile(const std::vector<std::shared_ptr<GameObjectInterface>>& gameObjects)
{
	// 一番近い敵を見つけ出す
	enemy = nullptr;
	float distance = 0.0f;

	for (const auto& gameObject : gameObjects)
	{
		if (gameObject->getId() == EnemyAirplane::id)
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

void PlayerAirplane::onOutside()
{
	triggerExplosion();
	state = Airplane::State::EXIT;
}
