#include "XFileObjectBase.h"

XFileObjectBase::XFileObjectBase(CDirect3DXFile *_xfile)
	: xfile(_xfile), angle(0, 0, 0), trans(0, 0, 0)
{
	if (!xfile->GetMesh()) throw "CDirect3DXFile���Z�b�g����Ă��Ȃ��̂ł�...?";
	mat = { //�i�P�ʍs��ŏ������j
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	show();
	enable();
}

void XFileObjectBase::draw(const LPDIRECT3DDEVICE9& device) const
{
	if (!drawing) return;

	D3DXMATRIX world;

	D3DXMatrixIdentity(&world);
	device->SetTransform(D3DTS_WORLD, &world);
	xfile->Draw(device);
}

D3DXMATRIX XFileObjectBase::getMat() const
{
	return mat;
}
