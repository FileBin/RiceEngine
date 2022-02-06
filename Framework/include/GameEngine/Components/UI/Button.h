#pragma once

#include "Image.h"

typedef void (*Listener)();

namespace Game::UI {
	class Button : public Image {

		enum class ButtonState { DISABLED, NEUTRAL, HOVER, PRESSED };

	public:
		ButtonState getState() { return state; };
		void setOnClickListener(Listener click_listener) { on_click_listener = click_listener; };
		void setOnReleaseListener(Listener release_listener) { on_release_listener = release_listener; };
		void setOnHoverInListener(Listener hover_in_listener) { on_hover_in_listener = hover_in_listener; };
		void setOnHoverOutListener(Listener hover_out_listener) { on_hover_out_listener = hover_out_listener; };

		void OnInit();
		void Update();

		void Draw(Device* device);
	private:
		bool checkHover();
		void setState(ButtonState state);
		void fireListener(Listener listener, bool fire) {
			if (listener != NULL && fire) {
				listener();
			}
		};

		D2D1::ColorF neutral_color = D2D1::ColorF::White, hover_color = D2D1::ColorF(.7,.7,.7), press_color = D2D1::ColorF(.5, .5, .5);
		Microsoft::WRL::ComPtr<Buffer> PSConstBuffer;
		ButtonState state;
		ButtonState prev_state;
		Listener on_click_listener;
		Listener on_release_listener;
		Listener on_hover_in_listener;
		Listener on_hover_out_listener;

		static Shader* button_shader;
	};
}
