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
		Vector2 buttonTopRight = canvas->TransformPositionToScreen(transform->GetPosition2DWithAnchor(canvas));
		Vector2 buttonScale = canvas->TransformScaleToScreen(transform->GetScale2D());
		Vector2 anchor = transform->GetAnchor();
		anchor += Vector2::one;
		anchor *= .5;
		buttonTopRight += anchor * buttonScale;
		Vector2 testPosition = InputManager::GetMousePos();
		Log::log(Log::LogLevel::INFO, L"mousePos: ({:.2f}, {:.2f})", testPosition.x, testPosition.y);
		Log::log(Log::LogLevel::INFO, L"buttonPos: ({:.2f}, {:.2f})", buttonTopRight.x, buttonTopRight.y);

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