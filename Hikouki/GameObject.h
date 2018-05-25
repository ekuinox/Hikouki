#pragma once
#include "CDirect3DXFile.h"

class GameObjectInterface {
protected:
	bool drawing; // �`�悷�邩�̃t���O
	bool active; // �X�V���邩�̃t���O
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
	D3DXMATRIX mat; // �ώZ�s��
	CDirect3DXFile *xfile;
	D3DXVECTOR3 angle, trans; // �ړ���
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