﻿#pragma once
#include "Device.h"
#include "Stage.h"

namespace Game {

	class SoundManager;
	class ScriptBase;
	class Core;
	class RenderBase;
	class Material;
	class Scene;

	class Engine
	{
	public:
		Engine(Device** device, Core* Core, Stage* stage);
		~Engine();

		size_t GetMsaaLevel() const { return msaaLevel; }

		HWND GetHWND() { return (**device).GetHWND(); }

		void SetMsaaLevel(size_t level);

		void SetRender(RenderBase* render);

		void LoadScene(Scene* scene);

		void PostUpdate();
		
		double GetFixedDeltaTime();
		double GetDeltaTime();
		double GetTime();

		void RegisterScript(ScriptBase* script, Stage stage = Stage::Update);

		Material& CreateMaterial(Shader& sh);
		Shader& CreateShader();

		SoundManager& getSoundManager();

	private:
		Device** device = nullptr;
		Core* core;
		size_t msaaLevel = 1;
		Stage* stage;
		Scene* scene;
	};
}
