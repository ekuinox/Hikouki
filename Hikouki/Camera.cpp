#include "Camera.h"

trau::Camera::Camera(const D3DXVECTOR3 _eye, const D3DXVECTOR3 _lookat, const D3DXVECTOR3 _up, int _width, int _height)
	: eye(_eye), lookat(_lookat), up(_up), w(_width), h(_height)
{
}

trau::Camera::Camera(const D3DXVECTOR3 _eye, const D3DXVECTOR3 _lookat)
	: Camera(_eye, _lookat, {0, 1, 0}, width, height)
{
}

void trau::Camera::update()
{
	D3DXMatrixLookAtLH(&view, &eye, &lookat, &up);

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 2,					// 視野角
		static_cast<float>(w) / static_cast<float>(h),	// アスペクト比
		1.0f,							// ニアプレーン
		5000.0f							// ファープレーン
	);
}

const D3DXMATRIX& trau::Camera::getView() const
{
	return view;
}

const D3DXMATRIX& trau::Camera::getProj() const
{
	return proj;
}

void trau::Camera::set(LPDIRECT3DDEVICE9 device)
{
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);
}

trau::TPSCamera::TPSCamera()
	: Camera(D3DXVECTOR3(), D3DXVECTOR3())
{
}

void trau::TPSCamera::update(const D3DXMATRIX& target)
{
	lookat = D3DXVECTOR3(target._41, target._42, target._43);
	eye = lookat - 10 * D3DXVECTOR3(target._31, target._32, target._33);
	up = D3DXVECTOR3(target._21, target._22, target._23);
	
	trau::Camera::update();
}

trau::FPSCamera::FPSCamera()
	: Camera(D3DXVECTOR3(), D3DXVECTOR3())
{
}

void trau::FPSCamera::update(const D3DXMATRIX& target)
{
	eye = 2 * D3DXVECTOR3(target._41, target._42, target._43); // ずらさないと本体と被っちまうので
	lookat = eye + 10 * D3DXVECTOR3(target._31, target._32, target._33);
	up = D3DXVECTOR3(target._21, target._22, target._23);
	
	trau::Camera::update();
}

trau::OverCamera::OverCamera()
	: Camera(D3DXVECTOR3(), D3DXVECTOR3()), azimuth(0.0), elevation(90), distance(-100)
{
}

void trau::OverCamera::update()
{
	lookat = D3DXVECTOR3(0, 0, 0);
	eye = D3DXVECTOR3(
		distance * sin(elevation) * cos(azimuth),
		distance * cos(elevation),
		distance * sin(azimuth) * sin(elevation)
	);
	eye += lookat;
	up = D3DXVECTOR3(0, 1, 0);

	trau::Camera::update();
}
