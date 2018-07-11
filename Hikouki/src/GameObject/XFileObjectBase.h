#pragma once

#ifndef ___X_FILE_OBJECT_BASE_H
#define ___X_FILE_OBJECT_BASE_H

#include "GameObject.h"
#include "../Utils/CDirect3DXFile.h"

class XFileObjectBase : public GameObject {
protected:
	D3DXMATRIX mat; // �ώZ�s��
	CDirect3DXFile *xfile;
	D3DXVECTOR3 angle, trans; // �ړ���
public:
	XFileObjectBase(CDirect3DXFile*);
	virtual void draw(const LPDIRECT3DDEVICE9&) const;
	D3DXMATRIX getMat() const;
	D3DXVECTOR3 getPos() const;
	const GameObjectInterface::Type getType() const;
};

#endif