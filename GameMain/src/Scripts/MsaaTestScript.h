#pragma once
#include <GameEngine\Core.h>
#include <GameEngine\InputManager.h>

using namespace Game;

class MsaaTestScript : public ScriptBase {
	bool playing = false;
	void Run() {
		auto& engine = GetEngine();
		if (InputManager::GetKey(KeyCode::Button1)) {
			//engine.SetMsaaLevel(1);
			if (!playing) {
				playing = true;
				engine.getSoundManager().play("test.ogg");
			}
		} else if (InputManager::GetKey(KeyCode::Button2)) {
			//engine.SetMsaaLevel(2);
		} else if (InputManager::GetKey(KeyCode::Button3)) {
			//engine.SetMsaaLevel(4);
		} else if (InputManager::GetKey(KeyCode::Button4)) {
			//engine.SetMsaaLevel(8);
		} else if (InputManager::GetKey(KeyCode::Button5)) {
			//engine.SetMsaaLevel(16);
		}
		//msaa brokes depth buffer

		if (InputManager::GetKey(KeyCode::Backspace)) {
			ShowWindow(GetConsoleWindow(), 1);
		}
	}
};
