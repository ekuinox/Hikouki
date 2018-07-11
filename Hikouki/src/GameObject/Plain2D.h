#pragma once

#ifndef ___PLANE_2D_H

#include "GameObject.h"

class Plain2D : public GameObject {
public:
	static constexpr auto FVF_VERTEX_2D = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	struct TextureSets {
		LPDIRECT3DTEXTURE9 image;
		float u, v, w, h;
	};
	using VERTEX = struct _VERTEX
	{
		float x, y, z;
		float rhw;
		D3DCOLOR diffuse;
		float u, v;
		_VERTEX() {};
		_VERTEX(float _x, float _y, float _u, float _v) : x(_x), y(_y), z(0.0f), u(_u), v(_v), rhw(1.0f), diffuse(D3DCOLOR_RGBA(255, 255, 255, 200)) {};
	};

	Plain2D(const D3DVECTOR& _center, const D3DVECTOR& _size, const TextureSets& _texture);
	virtual void draw(const LPDIRECT3DDEVICE9&) const;
	const GameObjectInterface::Type getType() const;
protected:
	D3DVECTOR center;
	D3DVECTOR size;
	TextureSets texture;
};

#define ___PLANE_2D_H
#endif