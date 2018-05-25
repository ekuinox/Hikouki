#include "Airplain.h"
#include "mathutil.h"

Airplain::Airplain(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(_xfile),
	explosion(new Explosion(xfile, device)), explosion_flag(false),
	bbox(new BoundingSphere(xfile->GetMesh(), device)), drawing_bbox(true)
{
}

Airplain::Airplain(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device, D3DXVECTOR3 coord)
	: Airplain(_xfile, device)
{
	D3DXMatrixTranslation(&mat, coord.x, coord.y, coord.z);
}

void Airplain::draw(LPDIRECT3DDEVICE9 device) const
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

void Airplain::update()
{
	if (!active) return;

	auto current = system_clock::now();
	if (std::chrono::duration_cast<std::chrono::microseconds>(current - latest_update).count() < 1) return;

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
		MakeWorldMatrix(mx, mat, angle, trans);
		bbox->updatePosition(mat);
	}

	latest_update = current;
}

void Airplain::startExplosion()
{
	if (explosion_flag) {
		explosion->TriangleTransforms(mat);
		explosion->Start();
	}
}

void Airplain::switchExplosion()
{
	explosion_flag = !explosion_flag;
	startExplosion();
}

void Airplain::switchDrawBBox()
{
	switchDrawBBox(!drawing_bbox);
}

void Airplain::switchDrawBBox(bool new_drawing_bbox)
{
	drawing_bbox = new_drawing_bbox;
}

BoundingSphere* Airplain::getBBox()
{
	return bbox;
}

void Airplain::addTrans(D3DXVECTOR3 v)
{
	trans += v;
}

void Airplain::setTrans(D3DXVECTOR3 v)
{
	trans = v;
}
