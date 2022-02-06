#pragma once

#include "Button.h"
#include <GameEngine\Components\UI\Text.h>

namespace Game::UI {

	class TextButton : public Button {

	public:
		TextButton(std::string texture, String text = L"Button", D2D1::ColorF color = D2D1::ColorF::White) {
			SceneObject& obj = GetSceneObject();
			ResourceManager& resman = obj.GetScene().GetResourceManager();
			SetImg(&obj.GetScene().GetRender().CreateTexture(resman.GetString(texture)));
			this->text = text;

			text_component = new Text();
			setText(text);
			setTextColor(color);
			text_component->PreInit(&obj);
		};

		void setText(String text) {
			text_component->SetText(text);
		}

		void setTextColor(D2D1::ColorF color) {
			text_component->SetColor(color);
		}

		void Draw(Device* device) {
			Button::Draw(device);
			text_component->Draw(device);
		}

	protected:
		String text;
		Text* text_component;
	};
}
