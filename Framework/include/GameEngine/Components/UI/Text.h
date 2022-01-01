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

		void SetFont(String fontName, IDWriteFontCollection* fontCollection = nullptr);

		void SetFontSize(float size);
		void SetFontWeight(DWRITE_FONT_WEIGHT weight);
		void SetFontStyle(DWRITE_FONT_STYLE style);
		void SetFontStretch(DWRITE_FONT_STRETCH stretch);
		void SetColor(D2D1::ColorF color);

		void Draw(IDWriteFactory* factory, IDWriteTextFormat* defformat, ID2D1RenderTarget* rt);
	private:
		String m_fontName = L"Arial";
		float m_fontSize = 14.f;
		DWRITE_FONT_WEIGHT m_weight = DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE m_style = DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH m_stretch = DWRITE_FONT_STRETCH_NORMAL;
		String m_text;
		Vector2 m_position = {0, 0}, m_size = { 100, 100 };
		D2D1::ColorF m_color = D2D1::ColorF::Black;
	};
}