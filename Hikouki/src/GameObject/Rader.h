#pragma once

#ifndef ___RADER_H

#include "XFileObjectBase.h"
#include <memory>
#include <vector>

class Rader : public GameObject {
public:
	static constexpr unsigned int id = GameObject::id * 100 + 6;
	
	Rader(const std::shared_ptr<XFileObjectBase>& _target, D3DXVECTOR2 _center, float _radius);
	void draw(const LPDIRECT3DDEVICE9&) const;
	virtual void update(const UpdateDetail&);
	virtual unsigned int getId() const { return id; }

private:
	struct VERTEX {
		float x, y, z;
		float rhw;
		DWORD color;
	};

	struct Dot {
		D3DXVECTOR2 coord;
		D3DXVECTOR2 size;
		DWORD color;
	};

	static constexpr int CIRCLED_VERTEXES_COUNT = 30; // �~�̒��_��
	static constexpr int SCOPE_RATE = 3; // ���W�̏k����

	D3DXVECTOR2 center;
	float radius;
	std::shared_ptr<XFileObjectBase> target;

	std::vector<Dot> dots;

	// �X�N���[�����W�ɕ`�悷��
	void drawTriangle(const D3DXVECTOR2& frontVertex, const D3DXVECTOR2& size, const DWORD& color, LPDIRECT3DDEVICE9 device) const;
	void drawQuad(const D3DXVECTOR2& frontVertex, const D3DXVECTOR2& size, const DWORD& color, LPDIRECT3DDEVICE9 device) const;
	void drawCircle(const D3DXVECTOR2& frontVertex, const float& radius, const DWORD& color, LPDIRECT3DDEVICE9 device) const;
	
	// �v���C�����S�̍��W�ɕϊ�����
	D3DXVECTOR3 fixCoordPlayer(const D3DXVECTOR3& wpos, const D3DXMATRIX& playerMatrix) const;

	// ���[�_�̕`��͈͂��ۂ����肷��
	bool isInsideRange(const D3DXVECTOR3& coord) const;
	
};

#define ___RADER_H
#endif
