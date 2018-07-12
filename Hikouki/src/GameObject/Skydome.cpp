#include "Skydome.h"

Skydome::Skydome(CDirect3DXFile * xfile, LPDIRECT3DDEVICE9 device)
	: XFileObjectBase(xfile), bbox(new BoundingSphere(xfile->GetMesh(), device))
{
	// 動かないしここで固定で．
	bbox->updatePosition(getMat());
}

void Skydome::update(const UpdateDetail & detail)
{
	// ここでいい感じに他のオブジェクトが範囲外にでないようにする
	auto r = bbox->getR();
}
