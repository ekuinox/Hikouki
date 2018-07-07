#include "Airplane.h"
#include "../Utils/MathUtil.h"

Airplane::Airplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, trau::Timer *timer)
	: XFileObjectBase(_xfile),
	explosion(new Explosion(xfile, device)), explosion_flag(false),
	bbox(new BoundingSphere(xfile->GetMesh(), device)), drawing_bbox(true), timer(timer)
{
}

Airplane::Airplane(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, D3DXVECTOR3 coord, trau::Timer *timer)
	: Airplane(_xfile, device, timer)
{
	D3DXMatrixTranslation(&mat, coord.x, coord.y, coord.z);
}

void Airplane::draw(const LPDIRECT3DDEVICE9& device) const
{
	if (!drawing) return;
	if (explosion_flag)
	{
		explosion->Draw(device);
		return;
	}
	device->SetTransform(D3DTS_WORLD, &mat);
	xfile->Draw(device);
	if (drawing_bbox) bbox->draw(device);
}

void Airplane::update()
{
	if (!active) return;

	if (explosion_flag)
	{
		explosion->Update();
	}
	else
	{
		D3DXMATRIX mx;

#ifdef __MOVE_RANDOM
		angle.x = rand() % 2 - 1;
		angle.y = rand() % 2 - 1;
#endif
		MakeWorldMatrix(mx, mat, angle, trans * timer->getSeconds());
		bbox->updatePosition(mat);
	}
}

void Airplane::startExplosion()
{
	if (explosion_flag) {
		explosion->TriangleTransforms(mat);
		explosion->Start();
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

BoundingSphere* Airplane::getBBox()
{
	return bbox;
}

void Airplane::addTrans(D3DXVECTOR3 v)
{
	trans += v;
}

void Airplane::setTrans(D3DXVECTOR3 v)
{
	trans = v;
}
