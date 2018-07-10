#include "BoundingSphere.h"
#include <initializer_list>

void BoundingSphere::createTranslucentMaterial(float alpha)
{
	material = {
		{ 1.0f, 1.0f, 1.0f, alpha },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
	};
}

void BoundingSphere::calculateBoundingSphere(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 device)
{
	HITDATA hit; // あたり判定データ格納構造体
	LPD3DXMESH clone_mesh;

	// あたり判定用データ作成のためにクローンメッシュを作成
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_XYZ, device, &clone_mesh)))
	{
		throw "Failed to create clone mesh";
	}
	else
	{
		LPDIRECT3DVERTEXBUFFER9 vertex_buffer; // 頂点バッファ
		
		BYTE *data; // 頂点バッファ内の頂点データの先頭アドレス

		auto vertexes_count = clone_mesh->GetNumVertices(); // 頂点数
		clone_mesh->GetVertexBuffer(&vertex_buffer);

		// 頂点バッファロック
		if (D3D_OK == vertex_buffer->Lock(0, 0, (void**)&data, D3DLOCK_READONLY))
		{
			D3DXVECTOR3 min, max; // 最小最大値
			D3DXVECTOR3 center_position; // 中心座標
			float max_distance = 0; // 最大距離

			// 頂点数分の座標格納エリア確保
			struct { float *x, *y, *z; } vertexes = { // 頂点座標データ
				new float[vertexes_count],
				new float[vertexes_count],
				new float[vertexes_count]
			};
			
			// 頂点をすべて取り出す
			auto work = (float*)data; // ワーク用のポインタ
			for (unsigned int i = 0; i < vertexes_count; i++)
			{
				// 座標の取得
				vertexes.x[i] = *work++;
				vertexes.y[i] = *work++;
				vertexes.z[i] = *work++;
			}

			// XYZについてそれぞれ最大値最小値を求める
			for (unsigned int i = 0; i < vertexes_count; i++)
			{
				if (i == 0)
				{
					min.x = max.x = vertexes.x[i];
					min.y = max.y = vertexes.y[i];
					min.z = max.z = vertexes.z[i];
				}
				if (min.x >= vertexes.x[i]) min.x = vertexes.x[i];
				if (min.y >= vertexes.y[i]) min.y = vertexes.y[i];
				if (min.x >= vertexes.z[i]) min.z = vertexes.z[i];
				if (max.x <= vertexes.x[i]) max.x = vertexes.x[i];
				if (max.y <= vertexes.y[i]) max.y = vertexes.y[i];
				if (max.x <= vertexes.z[i]) max.z = vertexes.z[i];
			}

			// 中心座標を求める
			center_position = {
				(max.x + min.x) / 2,
				(max.y + min.y) / 2,
				(max.z + min.z) / 2,
			};
			
			// 全頂点と中心座標との距離を求める
			for (unsigned int i = 0; i < vertexes_count; i++)
			{
				auto distance
					= (vertexes.x[i] - center_position.x) * (vertexes.x[i] - center_position.x)
					+ (vertexes.y[i] - center_position.y) * (vertexes.y[i] - center_position.y)
					+ (vertexes.z[i] - center_position.z) * (vertexes.z[i] - center_position.z)
					;
				if (max_distance < distance) max_distance = distance;
			}

			delete[] vertexes.x;
			delete[] vertexes.y;
			delete[] vertexes.z;
			
			vertex_buffer->Unlock();

			hit = {
				max,
				min,
				center_position,
				sqrtf(max_distance) // 最大半径を計算
			};
		}
	}
	clone_mesh->Release();
	hitdata = hit;
}

void BoundingSphere::createSphereMesh(float r, const LPDIRECT3DDEVICE9 device)
{
	D3DXCreateSphere(device, r, 20, 20, &sphere_mesh, nullptr);
}

void BoundingSphere::createBoxMesh(const LPDIRECT3DDEVICE9 device)
{
	D3DXCreateBox(
		device,
		fabsf(hitdata.max.x - hitdata.min.x),
		fabsf(hitdata.max.y - hitdata.min.y),
		fabsf(hitdata.max.z - hitdata.min.z),
		&box_mesh,
		nullptr
	);
}

float BoundingSphere::calculateDistance(D3DXVECTOR3 & a, D3DXVECTOR3 & b)
{
	return sqrtf(
		fabsf(powf(a.x - b.x, 2))
		+ fabsf(powf(a.y - b.y, 2))
		+ fabsf(powf(a.z - b.z, 2))
	);
}

BoundingSphere::BoundingSphere()
	: sphere_mesh(nullptr), box_mesh(nullptr)
{
}

BoundingSphere::BoundingSphere(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 device)
	: BoundingSphere()
{
	init(mesh, device);
}

BoundingSphere::~BoundingSphere()
{
	exit();
}

void BoundingSphere::init(const LPD3DXMESH mesh, const LPDIRECT3DDEVICE9 device)
{
	createTranslucentMaterial(0.3f); // 透明度の設定
	calculateBoundingSphere(mesh, device); // ヒットデータの計算
	createSphereMesh(hitdata.r, device); // 球メッシュを作成する
	createBoxMesh(device); // Boxメッシュを作成する
}

void BoundingSphere::updatePosition(const D3DXMATRIX & mat)
{
	auto pos = hitdata.pos;
	// 行列 * ベクトル
	D3DXVec3TransformCoord(&position, &pos, &mat);
	matrix = mat;
	matrix._41 = position.x;
	matrix._42 = position.y;
	matrix._43 = position.z;
}

void BoundingSphere::exit()
{
	if (sphere_mesh)
	{
		sphere_mesh->Release();
		sphere_mesh = nullptr;
	}
	if (box_mesh)
	{
		box_mesh->Release();
		box_mesh = nullptr;
	}
}

void BoundingSphere::draw(LPDIRECT3DDEVICE9 device)
{
	device->SetMaterial(&material);
	device->SetTexture(0, nullptr);
	device->SetTransform(D3DTS_WORLD, &matrix);
	sphere_mesh->DrawSubset(0);
}

void BoundingSphere::drawBox(LPDIRECT3DDEVICE9 device)
{
	device->SetMaterial(&material);
	device->SetTexture(0, nullptr);
	device->SetTransform(D3DTS_WORLD, &matrix);
	box_mesh->DrawSubset(0);
}

float BoundingSphere::getR() const
{
	return hitdata.r;
}

const D3DXVECTOR3 & BoundingSphere::getPosition()
{
	return position;
}

const BoundingSphere::HITDATA & BoundingSphere::getHitData()
{
	return hitdata;
}

bool BoundingSphere::isCollision(BoundingSphere * _another)
{
	// 半径の和と中心同士のの距離を比較する
	auto another = _another->getHitData();
	auto self = getHitData();
	auto another_position = _another->getPosition();

	auto distance = calculateDistance(position, another_position);

	return distance < (self.r + another.r);
}

