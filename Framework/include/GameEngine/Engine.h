#pragma once
#include "Device.h"
#include "Stage.h"

namespace Game {

	class SoundManager;
	class ScriptBase;
	class Core;
	class RenderBase;
	class Material;
	class Scene;
	class LoadingScreenRenderBase;

	class Engine
	{
	public:
		Engine(Device** device, Core* Core);
		~Engine();

		size_t GetMsaaLevel() const { return msaaLevel; }

		HWND GetHWND() { return (**device).GetHWND(); }

		void SetMsaaLevel(size_t level);

		void SetRender(LoadingScreenRenderBase* render);

		void LoadScene(Scene* scene);

		Device* GetDevice();

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
	};
}
