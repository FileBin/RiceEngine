#pragma once

#include "Button.h"
#include <GameEngine\Components\UI\Text.h>

namespace Game::UI {

	class TextButton : public Button {

	public:
		TextButton(String text = L"Button", D2D1::ColorF color = D2D1::ColorF::White) : Button() {
			
		};

		void setText(String text) {
			
		}

		void setTextColor(D2D1::ColorF color) {
			
		}

		void Draw(Device* device) {
			
		}

	protected:
		String text;
	};
}
