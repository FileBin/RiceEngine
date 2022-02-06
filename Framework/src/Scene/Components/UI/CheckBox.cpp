#include "pch.h"

#include <GameEngine\Components\UI\CheckBox.h>

namespace Game::UI {
	
	void CheckBox::Update() {
		Button::Update();
		if (prev_state == ButtonState::PRESSED && state == ButtonState::HOVER) {
			SetChecked(!isChecked, true);
		}
	}
}