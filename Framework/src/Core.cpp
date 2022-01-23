#include "pch.h"
#include <GameEngine/Core.h>
#include <GameEngine/macros.h>
#include <GameEngine/Device.h>
#include <GameEngine/ScriptBase.h>
#include <GameEngine/Engine.h>
#include <GameEngine/Util/exceptions.h>
#include <GameEngine/Util.h>
#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::this_thread;

namespace Game {

	void Core::RunNew(ScriptBase* script) {
		SetThreadPriority(GetCurrentThread(), 15);
		Core engine;
		try {
			engine.AddScript(script, Stage::PreInit);
			engine.Init();
			engine.Run();
			engine.Close();
		}
		#include "Util\ExeptionManager.h"
	}


	Core::Core() {}

	Core::~Core() {}
	bool Core::Init() {
		fps = 600;
		engine = new Engine(&device, this, &stage);
		device = new Device();
		stage = Stage::PreInit;
		RunScripts(preInitScripts);
		Log::PreInit();
		wnd = new Window();

		if (!wnd) {
			Log::log(Log::ERR, L"Window could not be constructed");
			return false;
		}

		DescWindow desc;
		if (!wnd->Create(desc)) {
			Log::log(Log::ERR, L"Window could not be created");
			return false;
		}

		auto hwnd = wnd->GetHWND();
		device->Init(hwnd);

		stage = Stage::Init;
		RunScripts(initScripts);

		render->SetDevice(device);
		if (!render->Init()) {
			Log::log(Log::ERR, L"Render could not be initialized");
			return false;
		}
		init = true;
		stage = Stage::PostInit;
		RunScripts(postInitScripts);
		return true;
	}
	void Core::Run() {
		if (init) {
			auto fixedDeltaTime = 1000. / fps;
			auto interval = (long long)fixedDeltaTime - 1;
			auto deltaTime = fixedDeltaTime;
			auto _time = steady_clock::now();
			time = 0;
			auto b = false;

			time_point start = steady_clock::now();
			do {
				stage = Stage::Update;
				RunScripts(updateScripts);
				wnd->inputmgr->Update();
				b = RunFrame();
				engine->PostUpdate();

				sleep_until(_time);
				time = (steady_clock::now() - start).count() * .000001 * .001;
				deltaTime = fixedDeltaTime + .000001*(steady_clock::now() - _time).count();
				_time = steady_clock::now() + milliseconds(interval);
				deltaTime = Math::Min(deltaTime, 300.);
				this->deltaTime = deltaTime * .001;
				this->fixedDeltaTime = fixedDeltaTime * .001;
			} while (b);
		}
	}

	void Core::Close() {
		stage = Stage::Close;
		RunScripts(closeScripts);
		init = false;
		render->Shutdown();
		//_DELETE(render);
		_CLOSE(wnd);
		Log::Close();
	}

	bool Core::RunFrame() {
		wnd->RunEvent();

		wnd->inputmgr->SetActive(wnd->IsActive());

		if (wnd->IsExit())
			return false;

		if (wnd->IsResize()) {
			device->Resize();
			render->Resize();
			if (!activeScene.IsNull()) {
				activeScene->Resize();
			}
		}
		if (activeScene.IsNull() || !activeScene->isLoaded()) {
			render->BeginFrame();
			render->Draw({});
		} else {
			activeScene->Render();
		}
		render->EndFrame();

		return true;
	}

	void Core::RunScripts(std::vector<ScriptBase*>& scripts){
		for (auto s : scripts) { s->Run(); }
	}

	void Core::AddScript(ScriptBase* script, Stage stage) {
		script->PreInit(&engine, reinterpret_cast<RenderBase**>(&render));
		script->Init();
		switch (stage) {
		case Stage::PreInit:
			preInitScripts.push_back(script);
			break;
		case Stage::Init:
			initScripts.push_back(script);
			break;
		case Stage::PostInit:
			postInitScripts.push_back(script);
			break;
		case Stage::Update:
			updateScripts.push_back(script);
			break;
		case Stage::Close:
			closeScripts.push_back(script);
			break;
		default:
			THROW_INVALID_ARG_EXCEPTION("Core::Stage");
			break;
		}
	}
}