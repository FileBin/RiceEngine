#include <GameEngine\stdafx.h>
#include <GameEngine\Engine.h>
#include <GameEngine\Core.h>

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
		if (*stage == Stage::PreInit) {
			(**device).SetMsaa(level);
		}
		msaaLevel = level;
	}

	void Engine::RegisterScript(ScriptBase* script, Stage stage) { core->AddScript(script, stage); }
	void Engine::SetRender(RenderBase* render) { core->SetRender(render); }
}