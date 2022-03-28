#pragma once

#include "Image.h"

namespace Game::UI {

	class Button : public Image {

	protected:
		struct ConstBufferData {
			Vector4f state;
		};

		enum class ButtonState { DISABLED, NEUTRAL, HOVER, PRESSED };

		ButtonState state;
		ButtonState prev_state;

	public:
		typedef std::function<void(Game::UI::Button* button)> Listener;

		ButtonState getState() { return state; };
		void setOnClickListener(Listener click_listener) { on_click_listener = click_listener; };
		void setOnReleaseListener(Listener release_listener) { on_release_listener = release_listener; };
		void setOnHoverInListener(Listener hover_in_listener) { on_hover_in_listener = hover_in_listener; };
		void setOnHoverOutListener(Listener hover_out_listener) { on_hover_out_listener = hover_out_listener; };

		virtual void OnInit();
		void Update();

		virtual void Draw(Device* device);

	protected:
		bool checkHover();
		void setState(ButtonState state);

		virtual SmartPtr<Shader> getShader() { return tex_shader; }
		virtual void setShader(Shader* sh) { tex_shader = sh; }

		static Shader* tex_shader;

		D2D1::ColorF neutral_color = {1, 1, 1}, hover_color = D2D1::ColorF(.7, .7, .7), press_color = D2D1::ColorF(.5, .5, .5);
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
