#include "pch.h"

#include <GameEngine\Components\UI\Button.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\Log.h>

namespace Game::UI {

	void Button::Update() {
		if (InputManager::GetKey(KeyCode::MouseLeft)) {
			if (state == ButtonState::OVER) {
				setState(ButtonState::PRESSED);
				if (listener != NULL) {
					listener();
				}
			}
		} else {
			setState(ButtonState::OVER);
		}

		Vector2 testPosition = InputManager::GetMousePos();
		Log::log(Log::LogLevel::ERR, std::to_wstring(testPosition.x));
		Log::log(Log::LogLevel::ERR, std::to_wstring(testPosition.y));

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