#pragma once
#include "GameObject.h"
#include "BoundingSphere.h"
#include "explosion.h"
#include "Timer.h"

class Airplain : public XFileObjectBase {
private:
	bool drawing_bbox;
	bool explosion_flag;
	void startExplosion();
	Explosion* explosion;
	BoundingSphere *bbox;
	trau::Timer *timer;
public:
	Airplain(CDirect3DXFile*, LPDIRECT3DDEVICE9, trau::Timer*);
	Airplain(CDirect3DXFile*, LPDIRECT3DDEVICE9, D3DXVECTOR3, trau::Timer*);
	void draw(LPDIRECT3DDEVICE9) const;
	void update();
	void switchExplosion();
	void switchDrawBBox();
	void switchDrawBBox(bool);
	BoundingSphere* getBBox();
	void addTrans(D3DXVECTOR3 v);
	void setTrans(D3DXVECTOR3 v);
};