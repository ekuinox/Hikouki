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
	HITDATA hit; // �����蔻��f�[�^�i�[�\����
	LPD3DXMESH clone_mesh;

	// �����蔻��p�f�[�^�쐬�̂��߂ɃN���[�����b�V�����쐬
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_XYZ, device, &clone_mesh)))
	{
		throw "Failed to create clone mesh";
	}
	else
	{
		LPDIRECT3DVERTEXBUFFER9 vertex_buffer; // ���_�o�b�t�@
		
		BYTE *data; // ���_�o�b�t�@���̒��_�f�[�^�̐擪�A�h���X

		auto vertexes_count = clone_mesh->GetNumVertices(); // ���_��
		clone_mesh->GetVertexBuffer(&vertex_buffer);

		// ���_�o�b�t�@���b�N
		if (D3D_OK == vertex_buffer->Lock(0, 0, (void**)&data, D3DLOCK_READONLY))
		{
			D3DXVECTOR3 min, max; // �ŏ��ő�l
			D3DXVECTOR3 center_position; // ���S���W
			float max_distance = 0; // �ő勗��

			// ���_�����̍��W�i�[�G���A�m��
			struct { float *x, *y, *z; } vertexes = { // ���_���W�f�[�^
				new float[vertexes_count],
				new float[vertexes_count],
				new float[vertexes_count]
			};
			
			// ���_�����ׂĎ��o��
			auto work = (float*)data; // ���[�N�p�̃|�C���^
			for (unsigned int i = 0; i < vertexes_count; i++)
			{
				// ���W�̎擾
				vertexes.x[i] = *work++;
				vertexes.y[i] = *work++;
				vertexes.z[i] = *work++;
			}

			// XYZ�ɂ��Ă��ꂼ��ő�l�ŏ��l�����߂�
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

			// ���S���W�����߂�
			center_position = {
				(max.x + min.x) / 2,
				(max.y + min.y) / 2,
				(max.z + min.z) / 2,
			};
			
			// �S���_�ƒ��S���W�Ƃ̋��������߂�
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
				sqrtf(max_distance) // �ő唼�a���v�Z
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
	createTranslucentMaterial(0.3f); // �����x�̐ݒ�
	calculateBoundingSphere(mesh, device); // �q�b�g�f�[�^�̌v�Z
	createSphereMesh(hitdata.r, device); // �����b�V�����쐬����
	createBoxMesh(device); // Box���b�V�����쐬����
}

void BoundingSphere::updatePosition(const D3DXMATRIX & mat)
{
	auto pos = hitdata.pos;
	// �s�� * �x�N�g��
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
	// ���a�̘a�ƒ��S���m�̂̋������r����
	auto another = _another->getHitData();
	auto self = getHitData();
	auto another_position = _another->getPosition();

	auto distance = calculateDistance(position, another_position);

	return distance < (self.r + another.r);
}

