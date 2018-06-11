#include "TextArea.h"

trau::TextArea::TextArea(LPDIRECT3DDEVICE9 device, int _x, int _y, const std::string & _text)
	: rect(new RECT{ _x, _y, 0, 0 }), text(_text)
{
	active = drawing = SUCCEEDED(D3DXCreateFont(
		device, _size.height, _size.width, _thickness,
		0, // ミップマップレベルの数
		_is_italic, _charset,
		OUT_DEFAULT_PRECIS, // 常にＴＲＵＥＴＹＰＥのフォントを使う
		PROOF_QUALITY, // 出力品質
		FIXED_PITCH | FF_MODERN, // ピッチとファミリーインデックス
		_font_name, &font
	));
}

trau::TextArea::TextArea(LPDIRECT3DDEVICE9 device, int _x, int _y)
	: TextArea(device, _x, _y, std::string(""))
{
}

trau::TextArea::TextArea(LPDIRECT3DDEVICE9 device)
	: TextArea(device, _position.x, _position.y)
{
}

void trau::TextArea::draw(const LPDIRECT3DDEVICE9&) const
{
	if (!drawing) return;

	// サイズを計算
	font->DrawText(NULL, text.c_str(), -1, rect, DT_CALCRECT, NULL);
	// 描画
	font->DrawText(NULL, text.c_str(), -1, rect, DT_LEFT | DT_BOTTOM, _color);
}