#pragma once

#include "UIBase.h"
#include "TextDrawer.h"

namespace Game::UI {
	class Text : public UIBase {
	public:

		void SetFont(String fontName, IDWriteFontCollection* fontCollection = nullptr) { drawer.m_fontName = fontName; }

		void SetFontSize(float size) { drawer.m_fontSize = size; }
		void SetFontWeight(DWRITE_FONT_WEIGHT weight) { drawer.m_weight = weight; }
		void SetFontStyle(DWRITE_FONT_STYLE style) { drawer.m_style = style; }
		void SetFontStretch(DWRITE_FONT_STRETCH stretch) { drawer.m_stretch = stretch; }
		void SetColor(D2D1::ColorF color) { drawer.m_color = color; }

		void SetText(String newText) { drawer.m_text = newText; }

		void Draw(Device* device);
	private:
		TextDrawer drawer;
	};
}