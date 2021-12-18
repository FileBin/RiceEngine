#include <GameEngine\stdafx.h>
#include <GameEngine\Engine.h>
#include <GameEngine\Core.h>
#include <GameEngine\Material.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {

	Engine::Engine(Device** d, Core* c, Stage* s) {
		if (d != nullptr && c != nullptr) {
			device = d;
			core = c;
			stage = s;
		} else {
			throw std::exception("Engine can't be created, because device reference is null!");
		}
	}

	Engine::~Engine() {}

	void Engine::PostUpdate() {
		auto d = *device;

		if (msaaLevel != d->GetMsaaLevel()) {
			d->SetMsaa(msaaLevel);
			d->ReCreateSwapChain();
			Log::Debug(L"Msaa set to {}x", msaaLevel);
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
		auto m = new Material({}, **device, sh);
		return *m;
	}

	Shader& Engine::CreateShader() {
		auto sh = new Shader(*device);
		return *sh;
	}

	void Engine::SetRender(RenderBase* render) { core->SetRender(render); }
	void Engine::LoadScene(Scene* scene) {
		auto& c = *core;
		c.SetRender(&scene->GetRender());
		scene->Init(this);
		auto s = scene->GetScripts();
		c.AddScript(s[0], Stage::PostInit);
		c.AddScript(s[1], Stage::Update);
		c.AddScript(s[2], Stage::Update);
	}
}