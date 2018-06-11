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
		static constexpr struct { int width, height; } _size = { 0, 10 }; // フォントの高さと幅
		static constexpr auto _font_name = "ＭＳＰゴシック"; // フォントの名前
		static constexpr auto _thickness = FW_REGULAR; // フォントの太さ
		static constexpr auto _is_italic = false; // イタリック
		static constexpr auto _charset = SHIFTJIS_CHARSET; // 文字セット
		static constexpr auto _color = 0xff550033; // フォント色
		static constexpr struct { int x, y; } _position = { 0, 0 }; //初期位置

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