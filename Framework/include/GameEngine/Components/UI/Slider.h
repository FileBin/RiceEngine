#pragma once

#include "Button.h"

namespace Game::UI {

	class Slider : public Button {
	public:

		typedef void (*ProgressListener)(Game::UI::Slider* checkBox, dbl progress);

		void SetMax(dbl max) {
			maxVal = max;
		}

		void SetProgress(dbl progress, bool notify_listener);

		dbl GetProgress() { return progress; }

		void Update();

		void setOnProgressChangedListener(ProgressListener progress_changed_listener) { on_progress_changed_listener = progress_changed_listener; };

	private:
		dbl maxVal;
		dbl progress;
		dbl prev_progress;
		ProgressListener on_progress_changed_listener;

		void fireListener(ProgressListener listener, bool fire) {
			if (listener != NULL && fire) {
				listener(this, progress);
			}
		};
	};

}