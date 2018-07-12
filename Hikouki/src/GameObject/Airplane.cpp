#include "Airplane.h"
#include "../Utils/MathUtil.h"
#include "../GameObjectAttachments/Collider.h"

Airplane::Airplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(_xfile), explosion(new Explosion(xfile->GetMesh(), xfile->GetTextures()[0], device)),
	bbox(new BoundingSphere(xfile->GetMesh(), device)), drawingBBox(false), state(State::ALIVE)
{
}

Airplane::Airplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, const D3DXVECTOR3& coord)
	: Airplane(_xfile, device)
{
	D3DXMatrixTranslation(&mat, coord.x, coord.y, coord.z);
}

void Airplane::draw(const LPDIRECT3DDEVICE9& device) const
{
	if (!drawing) return;

	if (state == State::Explosion)
	{
		explosion->draw(device);
	}
	else
	{
		if (drawingBBox) bbox->draw(device);
		XFileObjectBase::draw(device);
	}
}

void Airplane::update(const UpdateDetail& detail)
{
	if (!active) return;

	if (state == State::Explosion)
	{
		explosion->update(detail);
	}
	else
	{
		mathutils::makeWorldMatrixTotal(mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
		bbox->updatePosition(mat);
	}
}

void Airplane::triggerExplosion()
{
	if (state == State::ALIVE)
	{
		explosion->triangleTransforms(getMat());
		state = State::Explosion;
	}
}

BoundingSphere* Airplane::getBBox()
{
	return bbox;
}

void Airplane::addTrans(const D3DXVECTOR3& v)
{
	trans += v;
}

void Airplane::setTrans(const D3DXVECTOR3& v)
{
	trans = v;
}

Airplane::State Airplane::getState()
{
	return state;
}
