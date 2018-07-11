#include "PlayerAirplane.h"
#include "../Utils/MathUtil.h"

PlayerAirplane::PlayerAirplane(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device, const D3DXVECTOR3 & coord)
	: Airplane(xfile, device, coord)
{
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

	D3DXMATRIX mx;

	mathutils::makeWorldMatrix(mx, mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
}
