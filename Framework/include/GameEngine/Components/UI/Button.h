#pragma once

#include "Image.h"

typedef void (*ClickListener)();

namespace Game::UI {
	class Button : public Image {

		enum class ButtonState {DISABLED, NEUTRAL, OVER, PRESSED};

	public:
		ButtonState getState() { return state; };
		void setOnClickListener(ClickListener listener) { this->listener = listener; };
		void update();
	private:
		void setState(ButtonState state) { this->state = state; };
		ButtonState state;
		ClickListener listener;
	};
}
