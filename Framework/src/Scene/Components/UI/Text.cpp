#include "pch.h"
#include <GameEngine/Components/UI/Text.h>
#include <GameEngine/Util.h>
#include <GameEngine/Scene/Scene.h>

using namespace Game::Util;

namespace Game::UI {

	void Text::Draw(Device* device) {
		IDWriteFactory* factory = device->GetWriteFactory();
		IDWriteTextFormat* defformat = device->GetDefFormat();
		ID2D1RenderTarget* rt = device->Get2DRenderTarget();

		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout { nullptr };

		auto len = m_text.length();

		Vector2f scale = canvas->TransformScaleToScreen(transform->GetScale2D());
		Vector2f pos = canvas->TransformPositionToScreen(transform->GetPosition2D());

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

		Microsoft::WRL::ComPtr<ID2D1Brush> brush { nullptr };

		ThrowIfFailed(rt->CreateSolidColorBrush(m_color, reinterpret_cast<ID2D1SolidColorBrush**>(brush.GetAddressOf())));

		rt->DrawTextLayout({ pos.x, pos.y }, layout.Get(), brush.Get());
	}
}