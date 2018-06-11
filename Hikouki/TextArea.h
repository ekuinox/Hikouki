#pragma once

#ifndef ___TEXT_AREA_H
#define ___TEXT_AREA_H

#include <string>
#include "GameObject.h"

namespace trau
{
	class TextArea : public GameObjectInterface
	{
	private:
		static constexpr struct { int width, height; } _size = { 0, 10 }; // �t�H���g�̍����ƕ�
		static constexpr auto _font_name = "�l�r�o�S�V�b�N"; // �t�H���g�̖��O
		static constexpr auto _thickness = FW_REGULAR; // �t�H���g�̑���
		static constexpr auto _is_italic = false; // �C�^���b�N
		static constexpr auto _charset = SHIFTJIS_CHARSET; // �����Z�b�g
		static constexpr auto _color = 0xff550033; // �t�H���g�F
		static constexpr struct { int x, y; } _position = { 0, 0 }; //�����ʒu

		LPD3DXFONT font;
		RECT *rect;
	public:
		TextArea(LPDIRECT3DDEVICE9, int, int, const std::string&);
		TextArea(LPDIRECT3DDEVICE9, int, int);
		TextArea(LPDIRECT3DDEVICE9);
		void draw(LPDIRECT3DDEVICE9) const;
		void update();
		void show();
		void hide();
		void enable();
		void disable();

		std::string text;
	};
}

#endif