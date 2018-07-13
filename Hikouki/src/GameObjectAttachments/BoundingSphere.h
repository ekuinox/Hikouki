#pragma once

#include <d3dx9.h>

class BoundingSphere {
public:
	using HITDATA = struct {
		D3DXVECTOR3 max, min, pos;
		float r;
	};
private:
	HITDATA hitdata;
	D3DXVECTOR3 position;
	D3DXMATRIX matrix;
	D3DMATERIAL9 material;
	LPD3DXMESH sphere_mesh;
	LPD3DXMESH box_mesh;

	void createTranslucentMaterial(float alpha);
	void calculateBoundingSphere(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 device);
	void createSphereMesh(float r, const LPDIRECT3DDEVICE9 device);
	void createBoxMesh(const LPDIRECT3DDEVICE9 device);
	float calculateDistance(D3DXVECTOR3 &a, D3DXVECTOR3 &b);
public:
	BoundingSphere();
	BoundingSphere(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 device);
	~BoundingSphere();
	void init(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 device);
	void updatePosition(const D3DXMATRIX &mat);
	void exit();
	void draw(LPDIRECT3DDEVICE9 device);
	void drawBox(LPDIRECT3DDEVICE9 device);
	float getR() const;
	const D3DXVECTOR3& getPosition();
	const HITDATA& getHitData();
	bool isCollision(BoundingSphere* _another);
};