#pragma once
#include "XFileObjectBase.h"
#include "Explosion.h"
#include "../GameObjectAttachments/BoundingSphere.h"
#include "../SceneAttachments/Timer.h"

class Airplane : public XFileObjectBase {
public:
	enum class State {
		ALIVE,
		Explosion,
		EXIT,
	};
protected:
	bool drawingBBox;
	Explosion* explosion;
	BoundingSphere *bbox;
	Airplane::State state;
	static constexpr auto explosionTimeSeconds = 2.0f;
	trau::Timer explosionTimer;
public:
	static constexpr unsigned int id = XFileObjectBase::id * 100 + 1;
	Airplane(CDirect3DXFile*, LPDIRECT3DDEVICE9);
	Airplane(CDirect3DXFile*, LPDIRECT3DDEVICE9, const D3DXVECTOR3&);
	virtual ~Airplane();
	void draw(const LPDIRECT3DDEVICE9&) const;
	virtual void update(const UpdateDetail&);
	void triggerExplosion();
	BoundingSphere* getBBox();
	void addTrans(const D3DXVECTOR3& v);
	void setTrans(const D3DXVECTOR3& v);
	Airplane::State getState();
	virtual unsigned int getId() const { return id; }
};