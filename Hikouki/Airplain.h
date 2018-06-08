#pragma once
#include "GameObject.h"
#include "BoundingSphere.h"
#include "explosion.h"

class Airplain : public XFileObjectBase {
private:
	bool drawing_bbox;
	bool explosion_flag;
	void startExplosion();
	Explosion* explosion;
	BoundingSphere *bbox;
public:
	Airplain(CDirect3DXFile*, LPDIRECT3DDEVICE9);
	Airplain(CDirect3DXFile*, LPDIRECT3DDEVICE9, D3DXVECTOR3);
	void draw(LPDIRECT3DDEVICE9) const;
	void update();
	void switchExplosion();
	void switchDrawBBox();
	void switchDrawBBox(bool);
	BoundingSphere* getBBox();
	void addTrans(D3DXVECTOR3 v);
	void setTrans(D3DXVECTOR3 v);
};