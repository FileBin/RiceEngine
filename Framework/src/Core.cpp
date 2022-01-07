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
		SetThreadPriority(GetCurrentThread(), 2);
		Core engine;
		try {
			engine.AddScript(script, Stage::PreInit);
			engine.Initialize();
			engine.Run();
			engine.Close();
		}
		#include "Util\ExeptionManager.h"
	}


	Core::Core() {}

	Core::~Core() {}
	bool Core::Initialize() {
		fps = 60;
		engine = new Engine(&device, this, &stage);
		device = new Device();
		stage = Stage::PreInit;
		RunScripts(preInitScripts);
		Log::Init();
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
		device->Initialize(hwnd);

		stage = Stage::Init;
		RunScripts(initScripts);

		render->SetDevice(device);
		if (!render->Init()) {
			Log::log(Log::ERR, L"Render device could not be initialized");
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
				b = frame();
				engine->PostUpdate();

				sleep_until(_time);
				time = (steady_clock::now() - start).count() * .000001 * .001;
				deltaTime = fixedDeltaTime + .000001*(steady_clock::now() - _time).count();
				_time = steady_clock::now() + milliseconds(interval);

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

	bool Core::frame() {
		wnd->RunEvent();

#ifndef _DEBUG
		if (!wnd->IsActive())
			return true;
#endif

		if (wnd->IsExit())
			return false;

		if (wnd->IsResize()) {
			device->Resize();
			render->Resize();
		}
		stage = Stage::PreRender;
		RunScripts(preRenderScripts);
		render->BeginFrame();
		stage = Stage::Render;
		RunScripts(renderScripts);
		if (!render->Draw())
			return false;
		render->EndFrame();
		stage = Stage::PostRender;
		RunScripts(postRenderScripts);
		return true;
	}

	void Core::RunScripts(std::vector<ScriptBase*>& scripts){
		for (auto s : scripts) { s->Run(); }
	}

	void Core::AddScript(ScriptBase* script, Stage stage) {
		script->Init(&engine, &render);
		script->Initialize();
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
		case Stage::PreRender:
			preRenderScripts.push_back(script);
			break;
		case Stage::Render:
			renderScripts.push_back(script);
			break;
		case Stage::PostRender:
			postRenderScripts.push_back(script);
			break;
		case Stage::Update:
			updateScripts.push_back(script);
			break;
		case Stage::Close:
			closeScripts.push_back(script);
			break;
		default:
			ThrowIfFailed(E_INVALIDARG);
			break;
		}
	}
}