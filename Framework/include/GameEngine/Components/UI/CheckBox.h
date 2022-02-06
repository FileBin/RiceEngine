#pragma once

#include "Button.h"

namespace Game::UI {

	class CheckBox : public Button {
	public:
	
		bool IsChecked() {
			return isChecked;
		}

		void Update();

		void SetCheckedImg(Texture2D* texture) { tex_checked = texture; }
		void SetUncheckedImg(Texture2D* texture) { tex_unchecked = texture; }
		void SetChecked(bool checked, bool notify_liistener) { 
			isChecked = checked;
			if (checked) {
				SetImg(tex_checked);
			} else {
				SetImg(tex_unchecked);
			}
			fireListener(on_checked_change_listener, notify_liistener);
		};

		void setOnCheckedChangedListener(Listener checked_change_listener) { on_checked_change_listener = checked_change_listener; };
	private:
		bool isChecked;

		Texture2D* tex_checked;
		Texture2D* tex_unchecked;

		Listener on_checked_change_listener;
	};
	
}