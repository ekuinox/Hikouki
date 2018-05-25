#include "Airplain.h"
#include "mathutil.h"

Airplain::Airplain(CDirect3DXFile* _xfile, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(_xfile),
	explosion(new Explosion(xfile, device)), explosion_flag(false),
	bbox(new BoundingSphere(xfile->GetMesh(), device))
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

	device->SetTransform(D3DTS_WORLD, &mat);
	xfile->Draw(device);
	bbox->draw(device);
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
#ifdef __MOVE_RANDOM
		D3DXVECTOR3 angle(0.0f, 0.0f, 0.0f), trans(0.0f, 0.0f, 1.0f);
		D3DXMATRIX mx;

		angle.x = rand() % 2 - 1;
		angle.y = rand() % 2 - 1;

		MakeWorldMatrix(mx, mat, angle, trans);
#endif
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
