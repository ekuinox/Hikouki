#include "Airplane.h"
#include "../Utils/MathUtil.h"
#include "../GameObjectAttachments/Collider.h"

Airplane::Airplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(_xfile), explosion_flag(false), explosion(new Explosion(xfile->GetMesh(), xfile->GetTextures()[0], device)),
	bbox(new BoundingSphere(xfile->GetMesh(), device)), drawing_bbox(true), state(State::ALIVE)
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
	if (explosion_flag)
	{
		explosion->draw(device);
		return;
	}
	device->SetTransform(D3DTS_WORLD, &mat);
	xfile->Draw(device);
	if (drawing_bbox) bbox->draw(device);
}

void Airplane::update(const UpdateDetail& detail)
{
	if (!active) return;

	for (const auto& gameObject : detail.gameObjects)
	{
		if (gameObject->getUUID() == getUUID()) break; // é©ï™ÇèúäO

		// Ç»Å`Å`Å`Å`Å`Å`Å`ÇÒÇ‡ÇÌÇ©ÇÁÇÒ

		const auto hashCode = typeid(*gameObject).hash_code();

		if (hashCode == typeid(Airplane).hash_code())
		{
			const auto& airplane = std::static_pointer_cast<Airplane>(gameObject);
			auto colls = Collider::getCollisions({ getBBox() }, { airplane->getBBox() });
		}
	}

	if (explosion_flag)
	{
		explosion->update(detail);
	}
	else
	{
		D3DXMATRIX mx;

		mathutils::makeWorldMatrix(mx, mat, angle * detail.timer->getSeconds(), trans * detail.timer->getSeconds());
		bbox->updatePosition(mat);
	}
}

void Airplane::startExplosion()
{
	if (explosion_flag) {
		state = State::Explosion;
		explosion->triangleTransforms(mat);
	}
}

void Airplane::switchExplosion()
{
	explosion_flag = !explosion_flag;
	startExplosion();
}

void Airplane::switchDrawBBox()
{
	switchDrawBBox(!drawing_bbox);
}

void Airplane::switchDrawBBox(bool new_drawing_bbox)
{
	drawing_bbox = new_drawing_bbox;
}

bool Airplane::triggerExplosion()
{
	explosion_flag = true;
	startExplosion();
	return explosion_flag;
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
