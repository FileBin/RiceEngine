#include "pch.h"
#include <Rice/Engine/Core.hpp>

using namespace std::chrono;
using namespace std::this_thread;

NSP_ENGINE_BEGIN

	void Core::runNew(ScriptBase* script) {
		SetThreadPriority(GetCurrentThread(), 15);
		Core engine;
		 {
			engine.AddScript(script, Stage::PreInit);
			engine.Init();
			engine.Run();
			engine.Close();
		}
	}


	Core::Core() {}

	Core::~Core() {}

	void Core::LoadScene(Scene* _scene) {
		if (!loadScene.IsNull()) return;
		loadScene = _scene;
	}

	void Core::LoadSceneImmediate() {
		if (loadScene.IsNull()) return;
		if (!activeScene.IsNull()) {
			activeScene->Close();
		}
		activeScene = loadScene;
		loadScene = nullptr;
		activeScene->PreInit(engine, device);
		Core::RunTask([this]() {
			activeScene->Init();
			activeScene->PostInit();
			});
	}

	bool Core::Init() {
		engine = new Engine(&device, this);
		device = new Device();
		stage = Stage::PreInit;
		RunScripts(preInitScripts);
		AL::Init();
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

		loadingScreenScene->PreInit(engine, device);
		loadingScreenScene->Init();
		loadingScreenScene->PostInit();

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
				LoadSceneImmediate();
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
		loadingScreenScene->Close();
		loadingScreenScene.Release();
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
			loadingScreenScene->Resize();
			if (!activeScene.IsNull()) {
				activeScene->Resize();
			}
		}
		if (activeScene.IsNull() || !activeScene->isLoaded()) {
			loadingScreenScene->Render();
		} else {
			activeScene->Render();
		}
		device->SwapBuffers();

		return true;
	}

	void Core::RunScripts(std::vector<ScriptBase*>& scripts){
		for (auto s : scripts) { s->Run(); }
	}

	void Core::AddScript(ScriptBase* script, Stage stage) {
		script->PreInit(&engine);
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