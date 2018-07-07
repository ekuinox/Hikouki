#include	"CDirect3DXFile.h"
#include	"Explosion.h"

Explosion::Explosion()
{
	vertex = nullptr;
	num_vertex = 0;
	connect = nullptr;
	num_face = 0;
	triangle = nullptr;
	tex = nullptr;
}

Explosion::Explosion(CDirect3DXFile * xobj, LPDIRECT3DDEVICE9 device)
	: Explosion()
{
	Init(xobj, device);
}

Explosion::~Explosion()
{
	Uninit();
}

void Explosion::Init(CDirect3DXFile * xobj, LPDIRECT3DDEVICE9 device)
{
	XfileToTriangle(xobj->GetMesh(), device);
	D3DXCreateTextureFromFile(device, "assets/koa_1.tga", &tex);
}

void Explosion::Uninit()
{
	// �������ꂽ�O�p�`��񂪂���΍폜����
	if (triangle != nullptr) {
		delete[] triangle;
		triangle = nullptr;
	}

	if (tex != nullptr) {
		tex->Release();
	}
}

void Explosion::Start()
{
	last_update = std::chrono::system_clock::now();
}

void Explosion::Update()
{
	auto now = std::chrono::system_clock::now(); // ���ݎ���
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();

	for (int i = 0; i < num_face; i++) {
		D3DXMATRIX mat;

		// ��]�p�x����s����쐬����
		D3DXMatrixIdentity(&mat);
		D3DXMatrixRotationX(&triangle[i].mat, triangle[i].xangle);
		D3DXMatrixRotationY(&triangle[i].mat, triangle[i].yangle);
		D3DXMatrixRotationZ(&triangle[i].mat, triangle[i].zangle);

		triangle[i].mat._41 = triangle[i].cx;
		triangle[i].mat._42 = triangle[i].cy;
		triangle[i].mat._43 = triangle[i].cz;

		// ��]�p�x�̑�������
		// �������R�[�f�B���O
		// �\�\�\�\�\ start -----------

		triangle[i].xangle += triangle[i].dxa;
		triangle[i].yangle += triangle[i].dya;
		triangle[i].zangle += triangle[i].dza;

		// �\�\�\�\�\ end -----------

		// �@�������ւ̈ړ�
		// �������R�[�f�B���O
		// �\�\�\�\�\ start -----------

		triangle[i].ny -= GRAVITY * s / 1000 / 10; // 10�͋C�����������ɂ���������ł��D�D


		triangle[i].cx += triangle[i].nx;
		triangle[i].cy += triangle[i].ny ;
		triangle[i].cz += triangle[i].nz;
		
		// �\�\�\�\�\ end -----------

		for (int j = 0; j < 3; j++) {
			unsigned char alpha = (unsigned char)((triangle[i].Vertex[j].col & 0xff000000) >> 24);
			/*
			if (alpha > 0) {
				triangle[i].Vertex[j].col -= 0x01000000;	// �A���t�@�l�̕ύX
			}
			else {
				triangle[i].Vertex[j].col = 0x00ffffff;	// �A���t�@�l��0�ȉ��Ȃ�0��
			}
			*/
		}

	}
	last_update = now; // �ŏI�X�V�ɑ�����ďI��

}

void Explosion::Draw(const LPDIRECT3DDEVICE9& device)
{
	device->SetTexture(0, tex);

	for (int i = 0; i < num_face; i++) {

		//���_�̌^���Z�b�g
		device->SetFVF(D3DFVFCUSTOM_VERTEX);
		// ���[���h�ϊ��s����Z�b�g
		device->SetTransform(D3DTS_WORLD, &triangle[i].mat);
		//�����_�����O
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &triangle[i].Vertex[0], sizeof(MyVertex));

	}
}

void Explosion::TriangleTransforms(const D3DXMATRIX &mat)
{
	int				i;
	MyVertex		*TransformWork;	// ���[���h���W�n�ɕϊ����邽�߂̃��[�N
	D3DXVECTOR3		temp;

	int				idx0, idx1, idx2;
	D3DXVECTOR3		normal;
	D3DXVECTOR3		p0p1;
	D3DXVECTOR3		p0p2;

	// �������ꂽ�O�p�`��񂪂���΍폜����
	if (triangle != nullptr) {
		delete[] triangle;
		triangle = nullptr;
	}

	TransformWork = new MyVertex[num_vertex];		// ���W�ϊ��p���[�N�𒸓_������������

													// ���_�����ׂĎ��o�����[���h���W�ɕϊ�����
	for (i = 0; i < num_vertex; i++) {
		TransformWork[i] = vertex[i];		// ���[�J�����W�����W�ϊ��p���[�N�ɃZ�b�g

		temp.x = vertex[i].x;
		temp.y = vertex[i].y;
		temp.z = vertex[i].z;

		D3DXVec3TransformCoord(&temp, &temp, &mat);	// ���W�ϊ�

		TransformWork[i].x = temp.x;				// ���W�l�̂݃��[���h���W�ŏ�������
		TransformWork[i].y = temp.y;
		TransformWork[i].z = temp.z;
	}

	triangle = new MyTriangle[num_face];			// �O�p�`�̖ʐ����I�u�W�F�N�g����

	for (i = 0; i < num_face; i++) {						// �R�p�`�|���S���������[�v
		idx0 = connect[i].idx[0];
		idx1 = connect[i].idx[1];
		idx2 = connect[i].idx[2];

		triangle[i].Vertex[0] = TransformWork[idx0];
		triangle[i].Vertex[0].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// �f�B�t���[�Y�l���Z�b�g����B�i�w�t�@�C���Ɋ܂܂�Ă��Ȃ��ׁj

		triangle[i].Vertex[1] = TransformWork[idx1];
		triangle[i].Vertex[1].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// �f�B�t���[�Y�l���Z�b�g����B�i�w�t�@�C���Ɋ܂܂�Ă��Ȃ��ׁj

		triangle[i].Vertex[2] = TransformWork[idx2];
		triangle[i].Vertex[2].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// �f�B�t���[�Y�l���Z�b�g����B�i�w�t�@�C���Ɋ܂܂�Ă��Ȃ��ׁj

		p0p1.x = triangle[i].Vertex[1].x - triangle[i].Vertex[0].x;
		p0p1.y = triangle[i].Vertex[1].y - triangle[i].Vertex[0].y;
		p0p1.z = triangle[i].Vertex[1].z - triangle[i].Vertex[0].z;

		p0p2.x = triangle[i].Vertex[2].x - triangle[i].Vertex[0].x;
		p0p2.y = triangle[i].Vertex[2].y - triangle[i].Vertex[0].y;
		p0p2.z = triangle[i].Vertex[2].z - triangle[i].Vertex[0].z;

		// �@�����v�Z
		D3DXVec3Cross(&normal, &p0p1, &p0p2);			// �O�ς��v�Z
		D3DXVec3Normalize(&normal, &normal);			// �P�ʃx�N�g����
		triangle[i].nx = normal.x;					// �@�����Z�b�g
		triangle[i].ny = normal.y;
		triangle[i].nz = normal.z;

		// �d�S�v�Z
		triangle[i].cx = (triangle[i].Vertex[0].x + triangle[i].Vertex[1].x + triangle[i].Vertex[2].x) / 3;
		triangle[i].cy = (triangle[i].Vertex[0].y + triangle[i].Vertex[1].y + triangle[i].Vertex[2].y) / 3;
		triangle[i].cz = (triangle[i].Vertex[0].z + triangle[i].Vertex[1].z + triangle[i].Vertex[2].z) / 3;

		// ���_���W�����_����Ƃ������W�ɕϊ�����
		triangle[i].Vertex[0].x -= triangle[i].cx;
		triangle[i].Vertex[0].y -= triangle[i].cy;
		triangle[i].Vertex[0].z -= triangle[i].cz;

		triangle[i].Vertex[1].x -= triangle[i].cx;
		triangle[i].Vertex[1].y -= triangle[i].cy;
		triangle[i].Vertex[1].z -= triangle[i].cz;

		triangle[i].Vertex[2].x -= triangle[i].cx;
		triangle[i].Vertex[2].y -= triangle[i].cy;
		triangle[i].Vertex[2].z -= triangle[i].cz;
	}

	delete[] TransformWork;
	return;
}

void Explosion::XfileToTriangle(LPD3DXMESH mesh, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9		lpVertexBuffer;	// ���_�o�b�t�@
	LPDIRECT3DINDEXBUFFER9		lpIndexBuffer;	// �C���f�b�N�X�o�b�t�@
	int							length;			// ���_�t�H�[�}�b�g���i�o�C�g�j

	HRESULT			hr;
	LPD3DXMESH		clonemesh;					// ���b�V���C���^�[�t�F�C�X

	BYTE			*data;
	MyVertex		*pVertex;
	WORD			*pIndex;

	// �����p�̃��b�V�����쐬
	hr = mesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1), device, &clonemesh);
	if (FAILED(hr)) {
		MessageBox(0, "error ", "error ", MB_OK);
	}

	num_vertex = clonemesh->GetNumVertices();		// ���_�����擾

	clonemesh->GetVertexBuffer(&lpVertexBuffer);	// ���_�o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^���Q�b�g
	num_face = clonemesh->GetNumFaces();			// �ʐ����Q�b�g
	clonemesh->GetIndexBuffer(&lpIndexBuffer);		// �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^���Q�b�g

	length = sizeof(MyVertex);

	// ���_�o�b�t�@���b�N
	hr = lpVertexBuffer->Lock(0, length, (void**)&data, D3DLOCK_READONLY); // �Q�[�����[�v�̒��Ń��b�N������ƃG���[�ɂȂ�
	if (hr == D3D_OK) {
		vertex = new MyVertex[num_vertex];
		pVertex = (MyVertex*)data;

		// ���_�����R�s�[
		memcpy(vertex, pVertex, sizeof(MyVertex)*(num_vertex)); // data�̓��b�N���Ă�Ԃ����L���łȂ��̂�

		length = sizeof(WORD);
		// �C���f�b�N�X�o�b�t�@���b�N
		hr = lpIndexBuffer->Lock(0, length, (void**)&pIndex, D3DLOCK_READONLY);

		if (hr == D3D_OK) {
			connect = new MyConnect[num_face];	// �ʐ����̌��������擾

			for (int i = 0; i < num_face; i++) {		// �R�p�`�|���S���������[�v
				connect[i].idx[0] = *pIndex++;
				connect[i].idx[1] = *pIndex++;
				connect[i].idx[2] = *pIndex++;
			}
			lpIndexBuffer->Unlock();				// �C���f�b�N�X�o�b�t�@���
		}
		// ���_�o�b�t�@���
		lpVertexBuffer->Unlock();
	}
	clonemesh->Release();
	return;
}