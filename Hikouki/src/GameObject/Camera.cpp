#include "Camera.h"
#include "../Utils/MathUtil.h"

Camera::Camera(const std::shared_ptr<XFileObjectBase>& _target, const int& _w, const int& _h)
	: eye(D3DXVECTOR3()), lookat(D3DXVECTOR3()), up(D3DXVECTOR3()), w(_w), h(_h)
	, azimuth(0.0), elevation(90), distance(-100)
	, state(State::OVER), target(_target)
{
}

void Camera::afterUpdate(const UpdateDetail& detail)
{
	switch (state)
	{
	case State::FPS:
		onFPS(detail, target->getMat());
		break;
	case State::TPS:
		onTPS(detail, target->getMat());
		break;
	case State::OVER:
		onOVER(detail);
		break;
	default:
		break;
	}

	D3DXMatrixLookAtLH(&view, &eye, &lookat, &up);

	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 2, static_cast<float>(w) / static_cast<float>(h), nearPlane, farPlane);
}

void Camera::beforeDraw(const LPDIRECT3DDEVICE9& device)
{
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);
}

const D3DXMATRIX& Camera::getView() const
{
	return view;
}

const D3DXMATRIX& Camera::getProj() const
{
	return proj;
}

void Camera::onFPS(const UpdateDetail& detail, D3DXMATRIX mat)
{
	if (detail.input->getTrigger(KeyCode::V))
	{
		state = State::TPS;
		return;
	}
	else if (detail.input->getTrigger(KeyCode::O))
	{
		state = State::OVER;
		return;
	}

	eye = D3DXVECTOR3(mat._41, mat._42 + 2, mat._43);
	lookat = eye + 10 * D3DXVECTOR3(mat._31, mat._32, mat._33);
	up = D3DXVECTOR3(mat._21, mat._22, mat._23);
}

void Camera::onTPS(const UpdateDetail& detail, D3DXMATRIX mat)
{
	if (detail.input->getTrigger(KeyCode::V))
	{
		state = State::FPS;
		return;
	}
	else if (detail.input->getTrigger(KeyCode::O))
	{
		state = State::OVER;
		return;
	}

	lookat = D3DXVECTOR3(mat._41, mat._42, mat._43);
	eye = lookat - 10 * D3DXVECTOR3(mat._31, mat._32, mat._33);
	up = D3DXVECTOR3(mat._21, mat._22, mat._23);
}

void Camera::onOVER(const UpdateDetail& detail)
{
	if (detail.input->getTrigger(KeyCode::V))
	{
		state = State::FPS;
		return;
	}

	// ƒJƒƒ‰‘€ì
	if (detail.input->getPress(KeyCode::UpArrow))
		elevation += 10.0f * detail.timer->getSeconds();

	if (detail.input->getPress(KeyCode::DownArrow))
		elevation -= 10.0f * detail.timer->getSeconds();

	if (detail.input->getPress(KeyCode::RightArrow))
		azimuth -= 10.0f * detail.timer->getSeconds();

	if (detail.input->getPress(KeyCode::LeftArrow))
		azimuth += 10.0f * detail.timer->getSeconds();

	if (detail.input->getPress(KeyCode::Return) && 0 < distance)
		distance -= 0.1f;

	if (detail.input->getPress(KeyCode::BackSpace))
		distance += 0.1f;

	distance -= detail.input->getMouseState().lZ / 10;

	lookat = D3DXVECTOR3(0, 0, 0);
	eye = D3DXVECTOR3(
		distance * sin(elevation) * cos(azimuth),
		distance * cos(elevation),
		distance * sin(azimuth) * sin(elevation)
	);
	eye += lookat;
	up = D3DXVECTOR3(0, 1, 0);
}