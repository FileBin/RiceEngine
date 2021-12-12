#pragma once
#include "Device.h"
#include "Stage.h"

namespace Game {

	class ScriptBase;
	class Core;
	class RenderBase;

	class Engine
	{
	public:
		Engine(Device** device, Core* Core, Stage* stage);
		~Engine();

		size_t GetMsaaLevel() const { return msaaLevel; }
		void SetMsaaLevel(size_t level);

		void SetRender(RenderBase* render);

		void PostUpdate();

		void RegisterScript(ScriptBase* script, Stage stage = Stage::Update);

	private:
		Device** device = nullptr;
		Core* core;
		size_t msaaLevel = 1;
		Stage* stage;
	};
}
