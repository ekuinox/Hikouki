#include "Plain2D.h"

Plain2D::Plain2D(const D3DVECTOR & _center, const D3DVECTOR & _size, const TextureSets & _texture)
	: center(_center), size(_size), texture(_texture)
{
}

void Plain2D::draw(const LPDIRECT3DDEVICE9& device) const
{
	static constexpr auto vertexesSize = 4;
	static constexpr auto polygonsCount = 2;
	VERTEX vertexes[vertexesSize];

	for (auto i = 0; i < vertexesSize; ++i) {
		vertexes[i] = {
			center.x + size.x / (i % 3 == 0 ? -2 : 2),
			center.y + size.y / (i < 2 ? -2 : 2),
			texture.u + (i % 3 == 0 ? 0 : texture.w),
			texture.v + (i < 2 ? 0 : texture.h)
		};
	}

	device->SetTexture(0, texture.image);
	device->SetFVF(FVF_VERTEX_2D);

	device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, polygonsCount, vertexes, sizeof(VERTEX));
}
