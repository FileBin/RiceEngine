#pragma once

#include "Button.h"

namespace Game::UI {

	class CheckBox : public Button {
	public:
	
		typedef void (*CheckListener)(Game::UI::CheckBox* checkBox, bool isChecked);

		bool IsChecked() {
			return isChecked;
		}

		void Update();

		void SetCheckedImg(Texture2D* texture) { tex_checked = texture; }
		void SetUncheckedImg(Texture2D* texture) { tex_unchecked = texture; }
		void SetChecked(bool checked, bool notify_listener);

		void setOnCheckedChangedListener(CheckListener checked_change_listener) { on_checked_change_listener = checked_change_listener; };
	private:
		bool isChecked;

		Texture2D* tex_checked;
		Texture2D* tex_unchecked;

		CheckListener on_checked_change_listener;
		
		void fireListener(CheckListener listener, bool fire) {
			if (listener != NULL && fire) {
				listener(this, isChecked);
			}
		};
	};
	
}