#include "pch.h"
#include <GameEngine/Components/UI/Text.h>
#include <GameEngine/Util.h>
#include <GameEngine/Scene/Scene.h>

using namespace Game::Util;

namespace Game::UI {
	void Text::OnInit() {}

	void Text::OnEnable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		ren.AddText(this);
	}

	void Text::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		ren.RemoveText(this);
	}

	void Text::Draw(IDWriteFactory* factory, IDWriteTextFormat* defformat, ID2D1RenderTarget* rt) {
		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout { nullptr };

		auto len = m_text.length();

		ThrowIfFailed(factory->CreateTextLayout(m_text.c_str(), len, defformat, (FLOAT)m_size.x, (FLOAT)m_size.y, layout.GetAddressOf()));
		DWRITE_TEXT_RANGE range{};
		range.startPosition = 0;
		range.length = len;

		ThrowIfFailed(layout->SetFontFamilyName(m_fontName.c_str(), range));
		ThrowIfFailed(layout->SetFontSize(m_fontSize, range));
		ThrowIfFailed(layout->SetFontStretch(m_stretch, range));
		ThrowIfFailed(layout->SetFontWeight(m_weight, range));
		ThrowIfFailed(layout->SetFontStyle(m_style, range));

		Microsoft::WRL::ComPtr<ID2D1Brush> brush { nullptr };

		ThrowIfFailed(rt->CreateSolidColorBrush(m_color, reinterpret_cast<ID2D1SolidColorBrush**>(brush.GetAddressOf())));

		rt->DrawTextLayout({ (FLOAT)m_position.x, (FLOAT)m_position.y }, layout.Get(), brush.Get());
	}
}