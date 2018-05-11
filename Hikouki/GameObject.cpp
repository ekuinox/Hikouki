#include "GameObject.h"

XFileObjectBase::XFileObjectBase(CDirect3DXFile *_xfile)
	: xfile(_xfile)
{
	if (!xfile->GetMesh()) throw "CDirect3DXFileがセットされていないのでは...?";
	mat = { //（単位行列で初期化）
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

void XFileObjectBase::draw(LPDIRECT3DDEVICE9 device) const
{
	if (!drawing) return;

	D3DXMATRIX world;
	
	D3DXMatrixIdentity(&world);
	device->SetTransform(D3DTS_WORLD, &world);
	xfile->Draw(device);
}

void XFileObjectBase::update()
{
	if (!active) return;
}

void XFileObjectBase::show()
{
	drawing = true;
}

void XFileObjectBase::hide()
{
	drawing = false;
}

void XFileObjectBase::enable()
{
	active = true;
}

void XFileObjectBase::disable()
{
	active = false;
}
