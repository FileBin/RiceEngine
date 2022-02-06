#pragma once

#include "Image.h"

namespace Game::UI {

	class Button : public Image {

	protected:
		enum class ButtonState { DISABLED, NEUTRAL, HOVER, PRESSED };

		ButtonState state;
		ButtonState prev_state;

	public:
		//typedef void (*Listener)(Game::UI::Button* button);
		typedef std::function<void(Game::UI::Button* button)> Listener;

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

		D2D1::ColorF neutral_color = D2D1::ColorF::White, hover_color = D2D1::ColorF(.7, .7, .7), press_color = D2D1::ColorF(.5, .5, .5);
		Microsoft::WRL::ComPtr<Buffer> PSConstBuffer;

		Listener on_click_listener;
		Listener on_release_listener;
		Listener on_hover_in_listener;
		Listener on_hover_out_listener;
	
		void fireListener(Listener listener, bool fire) {
			if (listener != NULL && fire) {
				listener(this);
			}
		};
	};
}
