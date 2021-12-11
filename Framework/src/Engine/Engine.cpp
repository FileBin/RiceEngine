#include <GameEngine\stdafx.h>
#include <GameEngine\Engine.h>
#include <GameEngine\GameMain.h>

namespace GameEngine {

	Engine::Engine(Device** d, GameMain* c) {
		if (d != nullptr && c != nullptr) {
			device = d;
			core = c;
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

	void Engine::RegisterScript(ScriptBase& script, Stage stage) { core->AddScript(script, stage); }
}