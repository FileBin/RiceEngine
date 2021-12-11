#pragma once
#include "Device.h"
#include "Stage.h"

namespace GameEngine {

	class ScriptBase;
	class GameMain;

	class Engine
	{
	public:
		Engine(Device** device, GameMain* Core);
		~Engine();

		size_t GetMsaaLevel() const { return msaaLevel; }
		void SetMsaaLevel(size_t level) { msaaLevel = level; }

		void PostUpdate();

		void RegisterScript(ScriptBase& script, Stage stage = Stage::Update);

	private:
		Device** device = nullptr;
		GameMain* core;
		size_t msaaLevel = 1;
	};
}
