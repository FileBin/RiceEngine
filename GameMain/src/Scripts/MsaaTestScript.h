#pragma once
#include <GameEngine\Core.h>
#include <GameEngine\InputManager.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Game;

class MsaaTestScript : public ScriptBase {
	bool playing = false;
	void Run() {
		auto& engine = GetEngine();
		if (InputManager::GetKey(KeyCode::Button1)) {
			
			engine.SetMsaaLevel(1);
			if (!playing) {
				//std::vector<SoundEffect*> effects;
				//effects.push_back(new SoundEffect(Effect::REVERB));
				//effects.push_back(new SoundEffect(Effect::FREQUENCY_SHIFTER));
				//SmartPtr<SoundStream> stream = engine.getSoundManager().play_sound("test", 1, { 0, 0, 0 }, effects);
				//playing = true;
			}
			
			//Sleep(100);
			/*engine.getSoundManager().play_raw([](
				double time) -> char {
					return 127 * sin((2 * M_PI * 440 + 200 * abs(sin(abs(cos(abs(sin(time * 2)) * 2)) * 2))) * time); }, 0, 60, 1, { 0, 0, 0 });*/
			
		} else if (InputManager::GetKey(KeyCode::Button2)) {
			engine.SetMsaaLevel(2);
		} else if (InputManager::GetKey(KeyCode::Button3)) {
			engine.SetMsaaLevel(4);
		} else if (InputManager::GetKey(KeyCode::Button4)) {
			engine.SetMsaaLevel(8);
		} else if (InputManager::GetKey(KeyCode::Button5)) {
			engine.SetMsaaLevel(16);
		}

		if (InputManager::GetKey(KeyCode::Backspace)) {
			ShowWindow(GetConsoleWindow(), 1);
		}
	}
};
