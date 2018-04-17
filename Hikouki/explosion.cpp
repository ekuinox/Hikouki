//*****************************************************************************
//!	@file	explosion.cpp
//!	@brief	
//!	@note	爆発処理のための関数群
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	"CDirect3DXFile.h"
#include	"explosion.h"

//-----------------------------------------------------------------------------
//	グローバル変数
//-----------------------------------------------------------------------------
MyVertex	*g_LocalVertex = nullptr;			// 頂点情報
int			g_NumVertex=0;						// 頂点数
MyConnect	*g_Connect = nullptr;				// 結線情報
int			g_NumFace=0;						// 面数
MyTriangle	*g_pTriangle = nullptr;				// トライアングルの先頭
LPDIRECT3DTEXTURE9	g_pTex1 = nullptr;			// テクスチャ

//==============================================================================
//!	@fn		TriangleTransforms
//!	@brief	ばらばらな３角形頂点情報（ローカル座標）をワールド座標系に変換
//!	@param　ワールド変換行列(入力)　　	
//!	@retval	なし
//==============================================================================
void TriangleTransforms(const D3DXMATRIX& mat)
{
	int				i;
	MyVertex		*TransformWork;	// ワールド座標系に変換するためのワーク
	D3DXVECTOR3		temp;

	int				idx0, idx1, idx2;
	D3DXVECTOR3		normal;
	D3DXVECTOR3		p0p1;
	D3DXVECTOR3		p0p2;

	// 生成された三角形情報があれば削除する
	if (g_pTriangle != nullptr){
		delete[] g_pTriangle;
		g_pTriangle = nullptr;
	}

	TransformWork = new MyVertex[g_NumVertex];		// 座標変換用ワークを頂点数分生成する

	// 頂点をすべて取り出しワールド座標に変換する
	for (i = 0; i<g_NumVertex; i++){
		TransformWork[i] = g_LocalVertex[i];		// ローカル座標を座標変換用ワークにセット

		temp.x = g_LocalVertex[i].x;
		temp.y = g_LocalVertex[i].y;
		temp.z = g_LocalVertex[i].z;

		D3DXVec3TransformCoord(&temp, &temp, &mat);	// 座標変換

		TransformWork[i].x = temp.x;				// 座標値のみワールド座標で書き換え
		TransformWork[i].y = temp.y;
		TransformWork[i].z = temp.z;
	}

	g_pTriangle = new MyTriangle[g_NumFace];			// 三角形の面数分オブジェクト生成

	for (i = 0; i<g_NumFace; i++){						// ３角形ポリゴン数分ループ
		idx0 = g_Connect[i].idx[0];
		idx1 = g_Connect[i].idx[1];
		idx2 = g_Connect[i].idx[2];

		g_pTriangle[i].Vertex[0] = TransformWork[idx0];
		g_pTriangle[i].Vertex[0].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）

		g_pTriangle[i].Vertex[1] = TransformWork[idx1];
		g_pTriangle[i].Vertex[1].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）

		g_pTriangle[i].Vertex[2] = TransformWork[idx2];
		g_pTriangle[i].Vertex[2].col = D3DCOLOR_ARGB(255, 255, 255, 255);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）

		p0p1.x = g_pTriangle[i].Vertex[1].x - g_pTriangle[i].Vertex[0].x;
		p0p1.y = g_pTriangle[i].Vertex[1].y - g_pTriangle[i].Vertex[0].y;
		p0p1.z = g_pTriangle[i].Vertex[1].z - g_pTriangle[i].Vertex[0].z;

		p0p2.x = g_pTriangle[i].Vertex[2].x - g_pTriangle[i].Vertex[0].x;
		p0p2.y = g_pTriangle[i].Vertex[2].y - g_pTriangle[i].Vertex[0].y;
		p0p2.z = g_pTriangle[i].Vertex[2].z - g_pTriangle[i].Vertex[0].z;

		// 法線情報計算
		D3DXVec3Cross(&normal, &p0p1, &p0p2);			// 外積を計算
		D3DXVec3Normalize(&normal, &normal);			// 単位ベクトル化
		g_pTriangle[i].nx = normal.x;					// 法線情報セット
		g_pTriangle[i].ny = normal.y;
		g_pTriangle[i].nz = normal.z;

		// 重心計算
		g_pTriangle[i].cx = (g_pTriangle[i].Vertex[0].x + g_pTriangle[i].Vertex[1].x + g_pTriangle[i].Vertex[2].x) / 3;
		g_pTriangle[i].cy = (g_pTriangle[i].Vertex[0].y + g_pTriangle[i].Vertex[1].y + g_pTriangle[i].Vertex[2].y) / 3;
		g_pTriangle[i].cz = (g_pTriangle[i].Vertex[0].z + g_pTriangle[i].Vertex[1].z + g_pTriangle[i].Vertex[2].z) / 3;

		// 頂点座標を原点を基準とした座標に変換する
		g_pTriangle[i].Vertex[0].x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[0].y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[0].z -= g_pTriangle[i].cz;

		g_pTriangle[i].Vertex[1].x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[1].y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[1].z -= g_pTriangle[i].cz;

		g_pTriangle[i].Vertex[2].x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[2].y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[2].z -= g_pTriangle[i].cz;
	}

	delete[] TransformWork;
	return;
}

//==============================================================================
//!	@fn		XfileToTriangle
//!	@brief	Ｘファイルをばらばらな３角形プリミティブにばらす
//!	@param　メッシュオブジェクトポインタ(入力)　　	
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void XfileToTriangle(LPD3DXMESH lpmesh, LPDIRECT3DDEVICE9 lpdevice)
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
	hr = lpmesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1), lpdevice, &clonemesh);
	if (FAILED(hr)){
		MessageBox(0, "error ", "error ", MB_OK);
	}

	g_NumVertex = clonemesh->GetNumVertices();		// 頂点数を取得

	clonemesh->GetVertexBuffer(&lpVertexBuffer);	// 頂点バッファオブジェクトへのポインタをゲット
	g_NumFace = clonemesh->GetNumFaces();			// 面数をゲット
	clonemesh->GetIndexBuffer(&lpIndexBuffer);		// インデックスバッファオブジェクトへのポインタをゲット

	length = sizeof(MyVertex);

	// 頂点バッファロック
	hr = lpVertexBuffer->Lock(0, length, (void**)&data, D3DLOCK_READONLY);
	if (hr == D3D_OK){
		g_LocalVertex = new MyVertex[g_NumVertex];
		pVertex = (MyVertex*)data;

		// 頂点情報をコピー
		memcpy(g_LocalVertex, pVertex, sizeof(MyVertex)*(g_NumVertex));

		length = sizeof(WORD);
		// インデックスバッファロック
		hr = lpIndexBuffer->Lock(0, length, (void**)&pIndex, D3DLOCK_READONLY);

		if (hr == D3D_OK){
			g_Connect = new MyConnect[g_NumFace];	// 面数分の結線情報を取得

			for (int i = 0; i<g_NumFace; i++){		// ３角形ポリゴン数分ループ
				g_Connect[i].idx[0] = *pIndex++;
				g_Connect[i].idx[1] = *pIndex++;
				g_Connect[i].idx[2] = *pIndex++;
			}
			lpIndexBuffer->Unlock();				// インデックスバッファ解放
		}
		// 頂点バッファ解放
		lpVertexBuffer->Unlock();
	}
	clonemesh->Release();
	return;
}

//==============================================================================
//!	@fn		ExplosionInit
//!	@brief	爆発のための初期処理を行う
//!	@param　Xファイルオブジェクトポインタ(入力)　　	
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void ExplosionInit(CDirect3DXFile*	xobj,LPDIRECT3DDEVICE9 lpdevice){
	// Xファイルをバラバラな３角形に分割する
	XfileToTriangle(xobj->GetMesh(), lpdevice);
	D3DXCreateTextureFromFile(lpdevice, "assets/koa_1.tga", &g_pTex1);
}

//==============================================================================
//!	@fn		ExplosionUpdate
//!	@brief	爆発の更新処理
//!	@param　　　	
//!	@retval	なし
//==============================================================================
void ExplosionUpdate(){
	D3DXMATRIX mat1, mat2, mat3;

	for (int i = 0; i < g_NumFace; i++){

		// 回転角度から行列を作成する
		D3DXMatrixRotationX(&mat1, g_pTriangle[i].xangle);
		D3DXMatrixRotationY(&mat2, g_pTriangle[i].yangle);
		D3DXMatrixRotationZ(&mat3, g_pTriangle[i].zangle);

		g_pTriangle[i].mat = mat1*mat2*mat3;

		g_pTriangle[i].mat._41 = g_pTriangle[i].cx;
		g_pTriangle[i].mat._42 = g_pTriangle[i].cy;
		g_pTriangle[i].mat._43 = g_pTriangle[i].cz;

		// 回転角度の足しこみ
		// ここをコーディング
		// ――――― start -----------


		// ――――― end -----------

		// 法線方向への移動
		// ここをコーディング
		// ――――― start -----------


		// ――――― end -----------

		for (int j = 0; j<3; j++){
			unsigned char alpha = (unsigned char)((g_pTriangle[i].Vertex[j].col & 0xff000000) >> 24);
			if (alpha > 0){
				g_pTriangle[i].Vertex[j].col -= 0x01000000;	// アルファ値の変更
			}
			else{
				g_pTriangle[i].Vertex[j].col = 0x00ffffff;	// アルファ値が0以下なら0に
			}
		}

	}
}

//==============================================================================
//!	@fn		ExplosionDraw
//!	@brief	爆発を描画する
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void ExplosionDraw(LPDIRECT3DDEVICE9 lpdevice){

	lpdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpdevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpdevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	lpdevice->SetRenderState(D3DRS_LIGHTING, true);			//   光源計算オン（頂点の色が有効になる）

	// テクスチャステージステート
	lpdevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpdevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	lpdevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	lpdevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpdevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpdevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	lpdevice->SetTexture(0, g_pTex1);

	for (int i = 0; i<g_NumFace; i++){

		//頂点の型をセット
		lpdevice->SetFVF(D3DFVFCUSTOM_VERTEX);
		// ワールド変換行列をセット
		lpdevice->SetTransform(D3DTS_WORLD, &g_pTriangle[i].mat);
		//レンダリング
		lpdevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &g_pTriangle[i].Vertex[0], sizeof(MyVertex));

	}
}


//==============================================================================
//!	@fn		ExplosionExit
//!	@brief	爆発処理の後始末
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void ExplosionExit(){

	// 生成された三角形情報があれば削除する
	if (g_pTriangle != nullptr){
		delete[] g_pTriangle;
		g_pTriangle = nullptr;
	}

	if (g_pTex1 != nullptr){
		g_pTex1->Release();
	}
}

//******************************************************************************
//	End of file.
//******************************************************************************
