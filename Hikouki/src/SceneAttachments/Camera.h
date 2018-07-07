#pragma once

#ifndef ___CAMERA_H
#define ___CAMERA_H

#include <memory>
#include <d3dx9.h>
#include "../GameObject/XFileObjectBase.h"

namespace trau
{
	static auto width = 1280;
	static auto height = 720;

	enum class CameraTypes : char {
		FPS = 0,
		TPS = 1,
		OVER = 2 // ÇÆÇÈÇÆÇÈÉJÉÅÉâ
	};

	constexpr CameraTypes operator++(CameraTypes& c, int)
	{
		auto current = c;

		switch (c)
		{
		case CameraTypes::FPS:
			c = CameraTypes::TPS;
			break;
		case CameraTypes::TPS:
			c = CameraTypes::OVER;
			break;
		case CameraTypes::OVER:
			c = CameraTypes::FPS;
			break;
		}
		return current;
	}

	class Camera
	{
	protected:
		D3DXVECTOR3 eye; // éãì_
		D3DXVECTOR3 lookat; // íçéãì_
		D3DXVECTOR3 up;
		D3DXMATRIX view;
		D3DXMATRIX proj;
		const int w, h;
	public:
		Camera(const D3DXVECTOR3, const D3DXVECTOR3, const D3DXVECTOR3, int, int);
		Camera(const D3DXVECTOR3, const D3DXVECTOR3);
		virtual void update();
		const D3DXMATRIX& getView() const;
		const D3DXMATRIX& getProj() const;
		void set(LPDIRECT3DDEVICE9);
	};

	class TPSCamera : public Camera
	{
	public:
		TPSCamera();
		void update(const D3DXMATRIX&);
	};

	class FPSCamera : public Camera
	{
	public:
		FPSCamera();
		void update(const D3DXMATRIX&);
	};

	class OverCamera : public Camera
	{
	public:
		OverCamera();
		void update();

		float azimuth; // ï˚à äp
		float elevation; // ã¬
		float distance; // ãóó£
	};
}

#endif