#include "Skydome.h"

Skydome::Skydome(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(xfile), bbox(new BoundingSphere(xfile->GetMesh(), device))
{
	// �����Ȃ��������ŌŒ�ŁD
	bbox->updatePosition(getMat());
}

void Skydome::update(const UpdateDetail & detail)
{
}

const std::unique_ptr<BoundingSphere>& Skydome::getBBox() const
{
	return bbox;
}
