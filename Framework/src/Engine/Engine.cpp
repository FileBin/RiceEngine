#include "pch.h"
#include <GameEngine\stdafx.h>
#include <GameEngine\Engine.h>
#include <GameEngine\Core.h>
#include <GameEngine\Material.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Util\exceptions.h>

namespace Game {

	Engine::Engine(Device** d, Core* c, Stage* s) {
		if (d != nullptr && c != nullptr) {
			device = d;
			core = c;
		} else {
			THROW_EXCEPTION("Engine can't be created, because device reference is null!");
		}
	}

	Engine::~Engine() {}

	void Engine::PostUpdate() {
		auto d = *device;

		if (msaaLevel != d->GetMsaaLevel()) {
			d->SetMsaa(msaaLevel);
			d->ReCreateSwapChain();
			Log::log(Log::INFO, L"Msaa set to {}x", msaaLevel);
		}
	}

	void Engine::SetMsaaLevel(size_t level) {
		auto& d = **device;
		if (!d.IsInitialized()) {
			d.SetMsaa(level);
		}
		msaaLevel = level;
	}

	double Engine::GetDeltaTime() { return core->GetDeltaTime(); }
	double Engine::GetTime() {	return core->GetTime(); }
	double Engine::GetFixedDeltaTime() { return core->GetFixedDeltaTime(); }

	void Engine::RegisterScript(ScriptBase* script, Stage stage) { core->AddScript(script, stage); }

	Material& Engine::CreateMaterial(Shader& sh) {
		auto m = new Material(**device, sh);
		return *m;
	}

	Shader& Engine::CreateShader() {
		auto sh = new Shader(*device);
		return *sh;
	}

	void Engine::SetRender(LoadingScreenRenderBase* render) { core->render = render; }

	SoundManager& Engine::getSoundManager() { return core->activeScene->GetSoundManager(); };

	void Engine::LoadScene(Scene* scene) {
		if (!core->activeScene.IsNull()) {
			core->activeScene->Close();
		}
		core->activeScene = scene;
		scene->GetRender().SetDevice(core->device);
		scene->PreInit(this);
		Core::RunTask([scene]() {
			scene->Init();
			scene->PostInit(); 
			});
	}
}