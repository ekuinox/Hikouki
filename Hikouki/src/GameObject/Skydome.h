#pragma once

#ifndef ___SKYDOME_H

#include "XFileObjectBase.h"
#include <memory>
#include "../GameObjectAttachments/BoundingSphere.h"

class Skydome : public XFileObjectBase {
public:
	static constexpr unsigned int id = XFileObjectBase::id * 100 + 3;
	Skydome(CDirect3DXFile*, LPDIRECT3DDEVICE9);
	void update(const UpdateDetail&);
	const std::unique_ptr<BoundingSphere>& getBBox() const;
	virtual unsigned int getId() const { return id; }
private:
	std::unique_ptr<BoundingSphere> bbox;
};

#define ___SKYDOME_H
#endif