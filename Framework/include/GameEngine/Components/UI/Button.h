#pragma once

#include "Image.h"

typedef void (*ClickListener)();

namespace Game::UI {
	class Button : public Image {

		enum class ButtonState { DISABLED, NEUTRAL, OVER, PRESSED };

	public:
		ButtonState getState() { return state; };
		void setOnClickListener(ClickListener listener) { this->listener = listener; };

		void OnInit();
		void Update();

		void Draw(Device* device);
	private:
		void checkHover();
		void setState(ButtonState state);

		D2D1::ColorF neutral_color = D2D1::ColorF::White, hover_color = D2D1::ColorF(.7,.7,.7), press_color = D2D1::ColorF(.5, .5, .5);
		Microsoft::WRL::ComPtr<Buffer> PSConstBuffer;
		ButtonState state;
		ClickListener listener;

		static Shader* button_shader;
	};
}
