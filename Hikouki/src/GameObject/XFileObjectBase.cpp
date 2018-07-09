#include "XFileObjectBase.h"

XFileObjectBase::XFileObjectBase(CDirect3DXFile *_xfile)
	: xfile(_xfile), angle(0, 0, 0), trans(0, 0, 0)
{
	if (!xfile->GetMesh()) throw "CDirect3DXFileがセットされていないのでは...?";
	mat = { //（単位行列で初期化）
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

	device->SetTransform(D3DTS_WORLD, &mat);
	xfile->Draw(device);
}

D3DXMATRIX XFileObjectBase::getMat() const
{
	return mat;
}

D3DXVECTOR3 XFileObjectBase::getPos() const
{
	return D3DXVECTOR3(mat._41, mat._42, mat._43);
}
