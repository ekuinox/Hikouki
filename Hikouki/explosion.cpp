#include	"CDirect3DXFile.h"
#include	"explosion.h"

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
	// 生成された三角形情報があれば削除する
	if (triangle != nullptr) {
		delete[] triangle;
		triangle = nullptr;
	}

	if (tex != nullptr) {
		tex->Release();
	}
}

void Explosion::Update()
{
	D3DXMATRIX mat1, mat2, mat3;

	for (int i = 0; i < num_face; i++) {

		// 回転角度から行列を作成する
		D3DXMatrixRotationX(&mat1, triangle[i].xangle);
		D3DXMatrixRotationY(&mat2, triangle[i].yangle);
		D3DXMatrixRotationZ(&mat3, triangle[i].zangle);

		triangle[i].mat = mat1 * mat2*mat3;

		triangle[i].mat._41 = triangle[i].cx;
		triangle[i].mat._42 = triangle[i].cy;
		triangle[i].mat._43 = triangle[i].cz;

		// 回転角度の足しこみ
		// ここをコーディング
		// ――――― start -----------


		// ――――― end -----------

		// 法線方向への移動
		// ここをコーディング
		// ――――― start -----------


		// ――――― end -----------

		for (int j = 0; j < 3; j++) {
			unsigned char alpha = (unsigned char)((triangle[i].Vertex[j].col & 0xff000000) >> 24);
			if (alpha > 0) {
				triangle[i].Vertex[j].col -= 0x01000000;	// アルファ値の変更
			}
			else {
				triangle[i].Vertex[j].col = 0x00ffffff;	// アルファ値が0以下なら0に
			}
		}

	}
}

void Explosion::Draw(LPDIRECT3DDEVICE9 device)
{
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetRenderState(D3DRS_LIGHTING, true);			//   光源計算オン（頂点の色が有効になる）

															// テクスチャステージステート
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	device->SetTexture(0, tex);

	for (int i = 0; i < num_face; i++) {

		//頂点の型をセット
		device->SetFVF(D3DFVFCUSTOM_VERTEX);
		// ワールド変換行列をセット
		device->SetTransform(D3DTS_WORLD, &triangle[i].mat);
		//レンダリング
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &triangle[i].Vertex[0], sizeof(MyVertex));

	}
}

void Explosion::TriangleTransforms(const D3DXMATRIX &mat)
{
	int				i;
	MyVertex		*TransformWork;	// ワールド座標系に変換するためのワーク
	D3DXVECTOR3		temp;

	int idx[3];// idx0, idx1, idx2;
	D3DXVECTOR3		normal;
	D3DXVECTOR3 p0ps[2];
	/*
	D3DXVECTOR3		p0p1;
	D3DXVECTOR3		p0p2;
	*/

	// 生成された三角形情報があれば削除する
	if (triangle != nullptr) {
		delete[] triangle;
		triangle = nullptr;
	}

	TransformWork = new MyVertex[num_vertex];		// 座標変換用ワークを頂点数分生成する

													// 頂点をすべて取り出しワールド座標に変換する
	for (i = 0; i < num_vertex; i++) {
		TransformWork[i] = vertex[i];		// ローカル座標を座標変換用ワークにセット

		temp.x = vertex[i].x;
		temp.y = vertex[i].y;
		temp.z = vertex[i].z;

		D3DXVec3TransformCoord(&temp, &temp, &mat);	// 座標変換

		TransformWork[i].x = temp.x;				// 座標値のみワールド座標で書き換え
		TransformWork[i].y = temp.y;
		TransformWork[i].z = temp.z;
	}

	triangle = new MyTriangle[num_face];			// 三角形の面数分オブジェクト生成

	for (i = 0; i < num_face; i++) {						// ３角形ポリゴン数分ループ
		
		for (auto j = 0; j < 3; j++) idx[j] = connect[i].idx[j];

		for (auto j = 0; j < 3; j++)
		{
			triangle[i].Vertex[j] = TransformWork[idx[j]];
			triangle[i].Vertex[j].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）
		}

		for (auto j = 1; j < 2; j++)
		{
			p0ps[j - 1].x = triangle[i].Vertex[j].x - triangle[i].Vertex[0].x;
			p0ps[j - 1].y = triangle[i].Vertex[j].y - triangle[i].Vertex[0].y;
			p0ps[j - 1].z = triangle[i].Vertex[j].z - triangle[i].Vertex[0].z;
		}

		// 法線情報計算
		D3DXVec3Cross(&normal, &p0ps[0], &p0ps[1]);			// 外積を計算
		D3DXVec3Normalize(&normal, &normal);			// 単位ベクトル化
		triangle[i].nx = normal.x;					// 法線情報セット
		triangle[i].ny = normal.y;
		triangle[i].nz = normal.z;

		// 重心計算
		triangle[i].cx = (triangle[i].Vertex[0].x + triangle[i].Vertex[1].x + triangle[i].Vertex[2].x) / 3;
		triangle[i].cy = (triangle[i].Vertex[0].y + triangle[i].Vertex[1].y + triangle[i].Vertex[2].y) / 3;
		triangle[i].cz = (triangle[i].Vertex[0].z + triangle[i].Vertex[1].z + triangle[i].Vertex[2].z) / 3;

		// 頂点座標を原点を基準とした座標に変換する
		for (auto j = 0; j < 3; j++)
		{
			triangle[i].Vertex[j].x -= triangle[i].cx;
			triangle[i].Vertex[j].y -= triangle[i].cy;
			triangle[i].Vertex[j].z -= triangle[i].cz;
		}
	}

	delete[] TransformWork;
	return;
}

void Explosion::XfileToTriangle(LPD3DXMESH mesh, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9		lpVertexBuffer;	// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9		lpIndexBuffer;	// インデックスバッファ
	int							length;			// 頂点フォーマット長（バイト）

	HRESULT			hr;
	LPD3DXMESH		clonemesh;					// メッシュインターフェイス

	BYTE			*data;
	MyVertex		*pVertex;
	WORD			*pIndex;

	// 爆発用のメッシュを作成
	hr = mesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1), device, &clonemesh);
	if (FAILED(hr)) {
		MessageBox(0, "error ", "error ", MB_OK);
	}

	num_vertex = clonemesh->GetNumVertices();		// 頂点数を取得

	clonemesh->GetVertexBuffer(&lpVertexBuffer);	// 頂点バッファオブジェクトへのポインタをゲット
	num_face = clonemesh->GetNumFaces();			// 面数をゲット
	clonemesh->GetIndexBuffer(&lpIndexBuffer);		// インデックスバッファオブジェクトへのポインタをゲット

	length = sizeof(MyVertex);

	// 頂点バッファロック
	hr = lpVertexBuffer->Lock(0, length, (void**)&data, D3DLOCK_READONLY);
	if (hr == D3D_OK) {
		vertex = new MyVertex[num_vertex];
		pVertex = (MyVertex*)data;

		// 頂点情報をコピー
		memcpy(vertex, pVertex, sizeof(MyVertex)*(num_vertex));

		length = sizeof(WORD);
		// インデックスバッファロック
		hr = lpIndexBuffer->Lock(0, length, (void**)&pIndex, D3DLOCK_READONLY);

		if (hr == D3D_OK) {
			connect = new MyConnect[num_face];	// 面数分の結線情報を取得

			for (int i = 0; i < num_face; i++) {		// ３角形ポリゴン数分ループ
				connect[i].idx[0] = *pIndex++;
				connect[i].idx[1] = *pIndex++;
				connect[i].idx[2] = *pIndex++;
			}
			lpIndexBuffer->Unlock();				// インデックスバッファ解放
		}
		// 頂点バッファ解放
		lpVertexBuffer->Unlock();
	}
	clonemesh->Release();
	return;
}
