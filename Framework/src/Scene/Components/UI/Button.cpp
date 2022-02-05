#include "pch.h"

#include <GameEngine\Components\UI\Button.h>
#include <GameEngine\InputManager.h>

namespace Game::UI {

	void Button::update() {
		if (InputManager::GetKey(KeyCode::MouseLeft)) {
			if (state == ButtonState::NEUTRAL) {
				setState(ButtonState::PRESSED);
			}
		}
		switch (state)
		{
		case ButtonState::DISABLED:
			
			break;
		case ButtonState::NEUTRAL:

			break;
		case ButtonState::OVER:

			break;
		case ButtonState::PRESSED:

			break;
		}
	}

}