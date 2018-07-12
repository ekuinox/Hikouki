#pragma once

#ifndef ___CAMERA_H

#include "XFileObjectBase.h"

class Camera : public GameObject {
public:
	enum class State
	{
		FPS,
		TPS,
		OVER
	};
	Camera(const std::shared_ptr<XFileObjectBase>&, const int&, const int&);
	void afterUpdate(const UpdateDetail&);
	void beforeDraw(const LPDIRECT3DDEVICE9&);
	const D3DXMATRIX& getView() const;
	const D3DXMATRIX& getProj() const;
	virtual unsigned int getId() const { return id; }
private:
	State state;
	std::shared_ptr<XFileObjectBase> target;
	D3DXVECTOR3 eye; // éãì_
	D3DXVECTOR3 lookat; // íçéãì_
	D3DXVECTOR3 up;
	D3DXMATRIX view;
	D3DXMATRIX proj;

	// for OVER
	float azimuth; // ï˚à äp
	float elevation; // ã¬
	float distance; // ãóó£

	int w, h;
	static constexpr auto nearPlane = 1.0f, farPlane = 5000.0f;

	void onFPS(const UpdateDetail&, const D3DXMATRIX&);
	void onTPS(const UpdateDetail&, const D3DXMATRIX&);
	void onOVER(const UpdateDetail&);
};

#define ___CAMERA_H
#endif
