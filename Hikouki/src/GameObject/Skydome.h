#pragma once

#ifndef ___SKYDOME_H

#include "XFileObjectBase.h"
#include <memory>
#include "../GameObjectAttachments/BoundingSphere.h"

class Skydome : public XFileObjectBase {
public:
	Skydome(CDirect3DXFile*, LPDIRECT3DDEVICE9);
	void update(const UpdateDetail&);
private:
	std::unique_ptr<BoundingSphere> bbox;
};

#define ___SKYDOME_H
#endif