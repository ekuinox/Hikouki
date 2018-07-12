#pragma once

#ifndef ___EXPLOSION_H

#include "GameObject.h"
#include "../Utils/Utils.h"

class Explosion : public GameObject {
public:
	static constexpr unsigned int id = GameObject::id + 4;

	static constexpr auto D3DFVFCUSTOM_VERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	static constexpr auto GRAVITY = 9.8f;

	Explosion(const LPD3DXMESH& mesh, const LPDIRECT3DTEXTURE9& tex, const LPDIRECT3DDEVICE9& device);
	~Explosion();
	void draw(const LPDIRECT3DDEVICE9&) const;
	void update(const UpdateDetail&);
	void triangleTransforms(const D3DXMATRIX& mat);
	virtual unsigned int getId() const { return id; }
private:
	struct Connect {
		WORD idx[3];
	};

	struct Vertex {
		float x, y, z;
		DWORD col;
		float tu, tv;
	};

	struct Triangle {
		Vertex vertexes[3]; // 3ŠpŒ`‚Ì’¸“_
		float xangle; // ‰ñ“]Šp“x‚w
		float yangle; // ‰ñ“]Šp“x‚x
		float zangle; // ‰ñ“]Šp“x‚y
		float dxa; // ‰ñ“]Šp“x‘•ª‚w
		float dya; // ‰ñ“]Šp“x‘•ª‚x
		float dza; // ‰ñ“]Šp“x‘•ª‚y
		float cx, cy, cz; // dSÀ•W
		float nx, ny, nz; // –@üƒxƒNƒgƒ‹

		D3DXMATRIX mat; // •ÏŠ·s—ñ
		Triangle()
			: xangle(0.0f), yangle(0.0f), zangle(0.0f)
			, dxa(static_cast<float>(trau::utils::randomEngine() / RAND_MAX * 0.4))
			, dya(static_cast<float>(trau::utils::randomEngine() / RAND_MAX * 0.4))
			, dza(static_cast<float>(trau::utils::randomEngine() / RAND_MAX * 0.4))
		{
			D3DXMatrixIdentity(&mat);
		}
	};

	Vertex * vertex;
	int numVertexes;
	Connect * connect;
	int numFaces;
	Triangle *triangle;
	LPDIRECT3DTEXTURE9 texture;

	void createTriangleFromMesh(const LPD3DXMESH& mesh, const LPDIRECT3DDEVICE9& device);
};

#define ___EXPLOSION_H
#endif
