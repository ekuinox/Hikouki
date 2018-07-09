#pragma once
#include "XFileObjectBase.h"
#include "Explosion.h"
#include "../SceneAttachments/Timer.h"
#include "../GameObjectAttachments/BoundingSphere.h"

class Airplane : public XFileObjectBase {
protected:
	bool drawing_bbox;
	bool explosion_flag;
	void startExplosion();
	Explosion* explosion;
	BoundingSphere *bbox;
	trau::Timer *timer;
public:
	Airplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, trau::Timer*);
	Airplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, D3DXVECTOR3, trau::Timer*);
	void draw(const LPDIRECT3DDEVICE9&) const;
	virtual void update();
	void switchExplosion();
	void switchDrawBBox();
	void switchDrawBBox(bool);
	BoundingSphere* getBBox();
	void addTrans(D3DXVECTOR3 v);
	void setTrans(D3DXVECTOR3 v);
};