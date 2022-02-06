#pragma once
#include "..\..\Util\defines.h"
#include "..\..\Device.h"
#include "Canvas.h"

namespace Game::UI {
	class TextDrawer {
	public:
		String m_fontName = L"Arial";
		float m_fontSize = 20.f;
		DWRITE_FONT_WEIGHT m_weight = DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE m_style = DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH m_stretch = DWRITE_FONT_STRETCH_NORMAL;
		String m_text;
		D2D1::ColorF m_color = D2D1::ColorF::Black;

		void Draw(Device* device, Canvas* canvas, Vector2 referencePosition, Vector2 referenceScale) {
			IDWriteFactory* factory = device->GetWriteFactory();
			IDWriteTextFormat* defformat = device->GetDefFormat();
			ID2D1RenderTarget* rt = device->Get2DRenderTarget();

			Microsoft::WRL::ComPtr<IDWriteTextLayout> layout{ nullptr };

			auto len = m_text.length();

			Vector2f scale = canvas->TransformScaleToScreen(referenceScale);
			Vector2f pos = canvas->TransformPositionToScreen(referencePosition);

			ThrowIfFailed(factory->CreateTextLayout(m_text.c_str(), len, defformat, scale.x, scale.y, layout.GetAddressOf()));
			DWRITE_TEXT_RANGE range{};
			range.startPosition = 0;
			range.length = len;

			auto fontSize = canvas->TransformScaleToScreen(m_fontSize);

			ThrowIfFailed(layout->SetFontFamilyName(m_fontName.c_str(), range));
			ThrowIfFailed(layout->SetFontSize(fontSize, range));
			ThrowIfFailed(layout->SetFontStretch(m_stretch, range));
			ThrowIfFailed(layout->SetFontWeight(m_weight, range));
			ThrowIfFailed(layout->SetFontStyle(m_style, range));

			Microsoft::WRL::ComPtr<ID2D1Brush> brush{ nullptr };

			ThrowIfFailed(rt->CreateSolidColorBrush(m_color, reinterpret_cast<ID2D1SolidColorBrush**>(brush.GetAddressOf())));

			rt->DrawTextLayout({ pos.x, pos.y }, layout.Get(), brush.Get());
		}
	};
}
