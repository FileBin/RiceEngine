#pragma once

#include "../../Scene/Component.h"
#include "../../Util/defines.h"
#include "../../Vectors/Vector2.h"

namespace Game::UI {
	class Text : public Component {
	public:
		void OnInit();

		void OnEnable();
		void OnDisable();

		void SetFont(String fontName, IDWriteFontCollection* fontCollection = nullptr) { m_fontName = fontName; }

		void SetFontSize(float size) { m_fontSize = size; }
		void SetFontWeight(DWRITE_FONT_WEIGHT weight) { m_weight = weight; }
		void SetFontStyle(DWRITE_FONT_STYLE style) { m_style = style; }
		void SetFontStretch(DWRITE_FONT_STRETCH stretch) { m_stretch = stretch; }
		void SetColor(D2D1::ColorF color) { m_color = color; }

		void SetText(String newText) { m_text = newText; }

		void Draw(IDWriteFactory* factory, IDWriteTextFormat* defformat, ID2D1RenderTarget* rt);
	private:
		String m_fontName = L"Arial";
		float m_fontSize = 20.f;
		DWRITE_FONT_WEIGHT m_weight = DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE m_style = DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH m_stretch = DWRITE_FONT_STRETCH_NORMAL;
		String m_text;
		Vector2 m_position = {0, 0}, m_size = { 100, 100 };
		D2D1::ColorF m_color = D2D1::ColorF::Black;
	};
}