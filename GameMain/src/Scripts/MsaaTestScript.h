#pragma once
#include <GameEngine\GameMain.h>
#include <GameEngine\InputManager.h>

using namespace GameEngine;

class MsaaTestScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();
		if (InputManager::GetKey(KeyCode::Button1)) {
			engine.SetMsaaLevel(1);
		} else if (InputManager::GetKey(KeyCode::Button2)) {
			engine.SetMsaaLevel(2);
		} else if (InputManager::GetKey(KeyCode::Button3)) {
			engine.SetMsaaLevel(4);
		}
	}
};
