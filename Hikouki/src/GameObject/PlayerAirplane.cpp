#include "PlayerAirplane.h"
#include "../Utils/MathUtil.h"
#include "../GameObjectAttachments/Collider.h"
#include "../GameObject/Skydome.h"

PlayerAirplane::PlayerAirplane(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device, const D3DXVECTOR3 & coord, CDirect3DXFile * homingMissileXFile, CDirect3DXFile * bulletXFile)
	: Airplane(xfile, device, coord), enemy(nullptr)
{
	static constexpr auto initSpeed = 50.0f;

	homingMissile = std::unique_ptr<HomingMissile>(new HomingMissile(homingMissileXFile, enemy, D3DX_PI * 45.0f / 180.0f, getPos(), D3DXVECTOR3{ 0, 0, 1 }, device));

	for (auto i = 0; i < bullets.max_size(); ++i) bullets[i] = std::unique_ptr<Bullet>(new Bullet(bulletXFile, device));

	trans.z = initSpeed;

	// x�������ɐi�߂�悤��
	D3DXMatrixRotationY(&mat, D3DX_PI * 90.0f / 180.0f);
}

PlayerAirplane::PlayerAirplane(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device, const D3DXVECTOR3 & coord)
	: PlayerAirplane(xfile, device, coord, xfile, xfile)
{
}

void PlayerAirplane::update(const UpdateDetail & detail)
{
	// ������ OR �R���e�i�ɕύX���������ꍇ��
	if (detail.message & (GameObjectInterface::MESSAGE_INITIALIZE | GameObjectInterface::MESSAGE_ADDED | GameObjectInterface::MESSAGE_REMOVED))
	{
		// enemies��ݒ肷��
		enemies.resize(0);

		for (const auto& gameObject : detail.gameObjects)
		{
			if (gameObject->getId() == Skydome::id) skydome = std::static_pointer_cast<Skydome>(gameObject);
			else if (gameObject->getId() == EnemyAirplane::id) enemies.emplace_back(std::static_pointer_cast<EnemyAirplane>(gameObject));
		}
	}

	if (!active) return;

	if (state == State::Explosion)
	{
		explosion->update(detail);
		explosionTimer.end();
		if (explosionTimer.getSeconds() > explosionTimeSeconds) state = State::EXIT;

		return;
	}

	static constexpr auto angleMax = 45.0f;
	static constexpr auto addAngle = 50.0f;

	// �[���ɖ߂�
	angle = D3DXVECTOR3(0, 0, 0);

	// ����
	if (detail.input->getPress(KeyCode::A) && angle.y > -angleMax) angle.y -= addAngle;
	if (detail.input->getPress(KeyCode::D) && angle.y < angleMax) angle.y += addAngle;
	if (detail.input->getPress(KeyCode::W) && angle.x > -angleMax) angle.x -= addAngle;
	if (detail.input->getPress(KeyCode::S) && angle.x < angleMax) angle.x += addAngle;

	// BBox��؂�ւ���
	if (detail.input->getTrigger(KeyCode::F5)) drawingBBox = !drawingBBox;

	// �ǔ��~�T�C���𔭎˂���
	if (homingMissile->getState() == HomingMissile::State::PAUSE && (detail.input->getTrigger(KeyCode::E) || detail.input->getMouseState().rgbButtons[1] & 0x80))
		triggerHomingMissile();

	// �e�𔭎˂���
	if (detail.input->getTrigger(KeyCode::Q) || detail.input->getMouseState().rgbButtons[0] & 0x80)
		triggerBullet();

	// �q�b�g���Ă����Ƃ��̏���
	if (homingMissile->getState() == HomingMissile::State::HIT) homingMissile->pause();

	// �X�J�C�h�[���̊O
	const auto& r = skydome->getBBox().get()->getR() - bbox->getR();
	if (fabs(getPos().x) > r || fabs(getPos().y) > r || fabs(getPos().z > r)) onOutside();

	mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());

	homingMissile->update(detail);
	for (const auto& bullet : bullets) bullet->update(detail);

	bbox->updatePosition(mat);
}

void PlayerAirplane::draw(const LPDIRECT3DDEVICE9 & device) const
{
	Airplane::draw(device);
	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) homingMissile->draw(device);
	for (const auto& bullet : bullets) bullet->draw(device);
}

std::vector<D3DXVECTOR3> PlayerAirplane::getHomingMissilePositions()
{
	std::vector<D3DXVECTOR3> result = {};

	if (homingMissile->getState() == HomingMissile::State::FOLLOWING) result.emplace_back(homingMissile->getPos());

	return result;
}

void PlayerAirplane::triggerHomingMissile()
{
	// ��ԋ߂��G�������o��
	enemy = nullptr;
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

	if (enemy != nullptr) homingMissile->trigger(enemy, getMat());
}

void PlayerAirplane::triggerBullet()
{
	bulletTriggerTimer.end();
	if (bulletTriggerTimer.getSeconds() < bulletTriggerSpanSeconds) return;

	// �g���Ă��Ȃ��e��T���āC���˂���

	for (const auto& bullet : bullets)
	{
		if (bullet->getState() == Bullet::State::PAUSE)
		{
			bullet->trigger(getMat());
			break;
		}
	}

	bulletTriggerTimer.start();
}

void PlayerAirplane::onOutside()
{
	triggerExplosion();
}
