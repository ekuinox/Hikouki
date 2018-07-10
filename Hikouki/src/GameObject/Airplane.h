#pragma once
#include "XFileObjectBase.h"
#include "Explosion.h"
#include "../GameObjectAttachments/BoundingSphere.h"

class Airplane : public XFileObjectBase {
protected:
	bool drawing_bbox;
	bool explosion_flag;
	void startExplosion();
	Explosion* explosion;
	BoundingSphere *bbox;
public:
	Airplane(CDirect3DXFile*, LPDIRECT3DDEVICE9);
	Airplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const D3DXVECTOR3&);
	void draw(const LPDIRECT3DDEVICE9&) const;
	virtual void update(const UpdateDetail&);
	void switchExplosion();
	void switchDrawBBox();
	void switchDrawBBox(bool);
	BoundingSphere* getBBox();
	void addTrans(const D3DXVECTOR3& v);
	void setTrans(const D3DXVECTOR3& v);
};