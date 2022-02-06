#include "pch.h"

#include <GameEngine\Components\UI\CheckBox.h>

namespace Game::UI {
	
	void CheckBox::Update() {
		Button::Update();
		if (prev_state == ButtonState::PRESSED && state == ButtonState::HOVER) {
			SetChecked(!isChecked, true);
		}
	}

	void CheckBox::SetChecked(bool checked, bool notify_listener) {
		isChecked = checked;
		if (checked) {
			SetImg(tex_checked);
		}
		else {
			SetImg(tex_unchecked);
		}
		fireListener(on_checked_change_listener, notify_listener);
	};
}