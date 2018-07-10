#include "Explosion.h"

Explosion::Explosion(const LPD3DXMESH& mesh, const LPDIRECT3DTEXTURE9& tex, const LPDIRECT3DDEVICE9& device, trau::Timer *_timer)
	: vertex(nullptr), numVertexes(0), connect(nullptr), numFaces(0), triangle(nullptr), texture(tex), timer(_timer)
{
	createTriangleFromMesh(mesh, device);
}

Explosion::~Explosion()
{
	delete[] triangle;

	// ‚±‚Á‚¿‚Å¶¬‚µ‚½‚í‚¯‚¶‚á‚È‚¢‚Ì‚ÅCtexture‚ÌŠJ•ú‚Ís‚í‚È‚¢
}

void Explosion::draw(const LPDIRECT3DDEVICE9& device) const
{
	device->SetTexture(0, texture);
	device->SetFVF(D3DFVFCUSTOM_VERTEX);
	for (auto i = 0; i < numFaces; ++i)
	{
		device->SetTransform(D3DTS_WORLD, &triangle[i].mat);
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, triangle[i].vertexes, sizeof(Vertex));
	}
}

void Explosion::update(const UpdateDetail& detail)
{
	D3DXMATRIX mat;

	for (auto i = 0; i < numFaces; ++i)
	{

		D3DXMatrixIdentity(&mat);
		D3DXMatrixRotationX(&triangle[i].mat, triangle[i].xangle);
		D3DXMatrixRotationY(&triangle[i].mat, triangle[i].yangle);
		D3DXMatrixRotationZ(&triangle[i].mat, triangle[i].zangle);

		triangle[i].mat._41 = triangle[i].cx;
		triangle[i].mat._42 = triangle[i].cy;
		triangle[i].mat._43 = triangle[i].cz;

		triangle[i].xangle += triangle[i].dxa;
		triangle[i].yangle += triangle[i].dya;
		triangle[i].zangle += triangle[i].dza;

		triangle[i].ny -= GRAVITY * timer->getSeconds();

		triangle[i].cx += triangle[i].nx;
		triangle[i].cy += triangle[i].ny;
		triangle[i].cz += triangle[i].nz;

		for (auto j = 0; j < 3; ++j)
		{
			if (static_cast<unsigned char>((triangle[i].vertexes[j].col & 0xff000000) >> 24) > 0)
				triangle[i].vertexes[j].col -= 0x01000000;
			else
				triangle[i].vertexes[j].col = 0x00ffffff;
		}
	}
}

void Explosion::triangleTransforms(const D3DXMATRIX& mat)
{
	Vertex * transformWork;
	D3DXVECTOR3 temp;

	int idx0, idx1, idx2;
	D3DXVECTOR3	normal;
	D3DXVECTOR3	p0p1;
	D3DXVECTOR3	p0p2;

	if (triangle != nullptr) {
		delete[] triangle;
		triangle = nullptr;
	}

	transformWork = new Vertex[numVertexes];

	for (auto i = 0; i < numVertexes; ++i) {
		transformWork[i] = vertex[i];

		temp.x = vertex[i].x;
		temp.y = vertex[i].y;
		temp.z = vertex[i].z;

		D3DXVec3TransformCoord(&temp, &temp, &mat);

		transformWork[i].x = temp.x;
		transformWork[i].y = temp.y;
		transformWork[i].z = temp.z;
	}

	triangle = new Triangle[numFaces];

	for (auto i = 0; i < numFaces; ++i)
	{						
		idx0 = connect[i].idx[0];
		idx1 = connect[i].idx[1];
		idx2 = connect[i].idx[2];

		triangle[i].vertexes[0] = transformWork[idx0];
		triangle[i].vertexes[0].col = D3DCOLOR_ARGB(255, 255, 255, 255);

		triangle[i].vertexes[1] = transformWork[idx1];
		triangle[i].vertexes[1].col = D3DCOLOR_ARGB(255, 255, 255, 255);

		triangle[i].vertexes[2] = transformWork[idx2];
		triangle[i].vertexes[2].col = D3DCOLOR_ARGB(255, 255, 255, 255);

		p0p1.x = triangle[i].vertexes[1].x - triangle[i].vertexes[0].x;
		p0p1.y = triangle[i].vertexes[1].y - triangle[i].vertexes[0].y;
		p0p1.z = triangle[i].vertexes[1].z - triangle[i].vertexes[0].z;

		p0p2.x = triangle[i].vertexes[2].x - triangle[i].vertexes[0].x;
		p0p2.y = triangle[i].vertexes[2].y - triangle[i].vertexes[0].y;
		p0p2.z = triangle[i].vertexes[2].z - triangle[i].vertexes[0].z;

		D3DXVec3Cross(&normal, &p0p1, &p0p2);
		D3DXVec3Normalize(&normal, &normal);
		triangle[i].nx = normal.x;
		triangle[i].ny = normal.y;
		triangle[i].nz = normal.z;

		triangle[i].cx = (triangle[i].vertexes[0].x + triangle[i].vertexes[1].x + triangle[i].vertexes[2].x) / 3;
		triangle[i].cy = (triangle[i].vertexes[0].y + triangle[i].vertexes[1].y + triangle[i].vertexes[2].y) / 3;
		triangle[i].cz = (triangle[i].vertexes[0].z + triangle[i].vertexes[1].z + triangle[i].vertexes[2].z) / 3;

		triangle[i].vertexes[0].x -= triangle[i].cx;
		triangle[i].vertexes[0].y -= triangle[i].cy;
		triangle[i].vertexes[0].z -= triangle[i].cz;

		triangle[i].vertexes[1].x -= triangle[i].cx;
		triangle[i].vertexes[1].y -= triangle[i].cy;
		triangle[i].vertexes[1].z -= triangle[i].cz;

		triangle[i].vertexes[2].x -= triangle[i].cx;
		triangle[i].vertexes[2].y -= triangle[i].cy;
		triangle[i].vertexes[2].z -= triangle[i].cz;
	}

	delete[] transformWork;
	return;
}

void Explosion::createTriangleFromMesh(const LPD3DXMESH& mesh, const LPDIRECT3DDEVICE9& device)
{
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;

	LPD3DXMESH clonedMesh;

	BYTE *data;
	Vertex *pVertex;
	WORD *pIndex;

	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVFCUSTOM_VERTEX, device, &clonedMesh))) throw std::runtime_error("[Explosion] CloneMesh Error");

	numVertexes = clonedMesh->GetNumVertices();

	clonedMesh->GetVertexBuffer(&vertexBuffer);
	numFaces = clonedMesh->GetNumFaces();
	clonedMesh->GetIndexBuffer(&indexBuffer);

	if (vertexBuffer->Lock(0, sizeof(Vertex), (void**)&data, D3DLOCK_READONLY) == D3D_OK)
	{
		vertex = new Vertex[numVertexes];
		pVertex = reinterpret_cast<Vertex*>(data);

		memcpy(vertex, pVertex, sizeof(Vertex) * (numVertexes));

		if (indexBuffer->Lock(0, sizeof(WORD), (void**)&pIndex, D3DLOCK_READONLY) == D3D_OK)
		{
			connect = new Connect[numFaces];

			for (int i = 0; i < numFaces; ++i)
			{
				connect[i].idx[0] = *pIndex++;
				connect[i].idx[1] = *pIndex++;
				connect[i].idx[2] = *pIndex++;
			}
			indexBuffer->Unlock();
		}

		vertexBuffer->Unlock();
	}
	clonedMesh->Release();
	return;
}