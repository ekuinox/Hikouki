#pragma once

#ifndef ___X_FILE_OBJECT_BASE_H
#define ___X_FILE_OBJECT_BASE_H

#include "GameObject.h"
#include "CDirect3DXFile.h"

class XFileObjectBase : public GameObject {
protected:
	D3DXMATRIX mat; // êœéZçsóÒ
	CDirect3DXFile *xfile;
	D3DXVECTOR3 angle, trans; // à⁄ìÆó 
public:
	XFileObjectBase(CDirect3DXFile*);
	virtual void draw(const LPDIRECT3DDEVICE9&) const;
	D3DXMATRIX getMat() const;
};

#endif