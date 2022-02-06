#include "pch.h"

#include <GameEngine\Components\UI\Slider.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\Log.h>

namespace Game::UI {

	void Slider::SetProgress(dbl progress, bool notify_listener) {
		prev_progress = this->progress;
		this->progress = progress;
		fireListener(on_progress_changed_listener, notify_listener && prev_progress != progress);
	}

	void Slider::Update() {
		Button::Update();
		if (state == ButtonState::PRESSED) {
			SetProgress(InputManager::GetMousePos().x, true);
			if (prev_progress != progress) {
				transform->SetPosition({ ((progress - 580 - 30)) * 1.25, 0 });
				Log::log(Log::LogLevel::INFO, L"{}", progress);
			}
		}
	}

}