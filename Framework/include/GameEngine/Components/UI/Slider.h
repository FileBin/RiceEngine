#pragma once

#include "Button.h"

namespace Game::UI {

	class Slider : public Image {
	protected:
		enum class SliderState { DISABLED, NEUTRAL, HOVER, PRESSED } state, prev_state;

	public:
		typedef std::function<void(Game::UI::Slider* slider, dbl progress)> ProgressListener;
		typedef std::function<void(Game::UI::Slider* slider)> Listener;

		SliderState getState() { return state; };
		void setOnClickListener(Listener click_listener) { on_click_listener = click_listener; };
		void setOnReleaseListener(Listener release_listener) { on_release_listener = release_listener; };
		void setOnHoverInListener(Listener hover_in_listener) { on_hover_in_listener = hover_in_listener; };
		void setOnHoverOutListener(Listener hover_out_listener) { on_hover_out_listener = hover_out_listener; };

		void SetBgImg(Texture2D* texture) { bg_tex = texture; }

		void SetProgress(dbl progress, bool notify_listener);
		dbl GetProgress() { return progress; }

		void OnInit();
		void Update();

		void Draw(Device* device);

		void setOnProgressChangedListener(ProgressListener progress_changed_listener) { on_progress_changed_listener = progress_changed_listener; };

	private:
		dbl progress;
		dbl prev_progress;
		const Vector2 direction = Vector2::right;
		ProgressListener on_progress_changed_listener;

		Texture2D* bg_tex;

		Vector2 getPosition(dbl progr);
		bool checkHover();
		void setState(SliderState state);

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

		void fireListener(ProgressListener listener, bool fire) {
			if (listener != NULL && fire) {
				listener(this, progress);
			}
		};
	};

}