#pragma once

#include "Button.h"
#include <GameEngine\Components\UI\Text.h>

namespace Game::UI {

	class TextButton : public Button {

	public:
		static TextButton* CreateNew(String text = L"Button", D2D1::ColorF color = D2D1::ColorF::White) {
			auto button = new TextButton();
			button->setText(text);
			button->setTextColor(color);
			button->drawer.horizontal_aligment = DWRITE_TEXT_ALIGNMENT_CENTER;
			button->drawer.vertical_aligment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			return button;
		};

		void SetFont(String fontName) {
			drawer.m_fontName = fontName;
		}

		void OnInit() {
			//auto device = GetSceneObject().GetScene().GetEngine().GetDevice();
			//drawer.Init(device);
			Button::OnInit();
		}

		void setText(String text) {
			drawer.m_text = text;
		}

		void setTextColor(D2D1::ColorF color) {
			drawer.m_color = color;
		}

		void Draw(Device* device) {
			Button::Draw(device);
			auto asp = GetSceneObject().GetScene().GetRender().GetAspectRatio();
			drawer.Draw(device, canvas, transform->GetPosition2DWithAnchor(canvas) - transform->GetScale2DTransformed(asp) * .5 + Vector2::up * (dbl)drawer.m_fontSize * .25, transform->GetReferenceScale2D());
		}

	protected:
		TextDrawer drawer;
	};
}
