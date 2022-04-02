#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Engine;
typedef SmartPtr<Engine> pEngine;

NSP_ENGINE_END

#pragma once
#include "Core.hpp"
#include "../GL/GraphicsManager.hpp"
#include "../Misc/Stage.hpp"

NSP_ENGINE_BEGIN

class Engine {
public:
	Engine(Graphics::pGraphicsManager gmanager, pCore Core);
	~Engine();

	size_t GetMsaaLevel() const {
		return msaaLevel;
	}

	HWND GetHWND() {
		return (**device).GetHWND();
	}

	void SetMsaaLevel(size_t level);

	void SetLoadingScene(Scene* scene);

	void LoadScene(Scene* scene);

	void SetFps(float fps = 60.f);

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
};\

NSP_ENGINE_END
