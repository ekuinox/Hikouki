#pragma once
#include "CDirect3DXFile.h"

class GameObjectInterface {
protected:
	bool drawing; // 描画するかのフラグ
	bool active; // 更新するかのフラグ
public:
	virtual void draw(LPDIRECT3DDEVICE9) const = 0;
	virtual void update() = 0;
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;
};

class XFileObjectBase : public GameObjectInterface {
protected:
	D3DXMATRIX mat; // 積算行列
	CDirect3DXFile *xfile;
	D3DXVECTOR3 angle, trans; // 移動量
public:
	XFileObjectBase(CDirect3DXFile*);
	void draw(LPDIRECT3DDEVICE9) const;
	void update();
	void show();
	void hide();
	void enable();
	void disable();
	D3DXMATRIX getMat() const;
};