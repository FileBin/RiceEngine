#include <GameEngine/Core.h>
#include <GameEngine/macros.h>
#include <GameEngine/Device.h>
#include <GameEngine/ScriptBase.h>
#include <GameEngine\Engine.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::this_thread;

namespace Game {

	void Core::RunNew(ScriptBase* script) {
		Core engine;
		engine.AddScript(script, Stage::PreInit);
		engine.Initialize();
		engine.Run();
		engine.Close();
	}

	Core::Core() {}

	Core::~Core() {}
	bool Core::Initialize() {
		engine = new Engine(&device, this, &stage);
		device = new Device();
		stage = Stage::PreInit;
		RunScripts(preInitScripts);
		Log::Init();
		wnd = new Window();

		if (!wnd) {
			Log::Err(L"Не удалось выделить память");
			return false;
		}

		DescWindow desc;
		if (!wnd->Create(desc)) {
			Log::Err(L"Не удалось создать окно");
			return false;
		}

		auto hwnd = wnd->GetHWND();
		device->Initialize(hwnd);

		stage = Stage::Init;
		RunScripts(initScripts);

		render->SetDevice(device);
		if (!render->Init()) {
			Log::Err(L"Не удалось создать рендер");
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
			auto interval = (long long)fixedDeltaTime;
			auto deltaTime = fixedDeltaTime;
			auto time = steady_clock::now();
			auto b = false;
			do {
				b = frame();
				stage = Stage::Update;
				RunScripts(updateScripts);
				engine->PostUpdate();

				sleep_until(time);
				deltaTime = fixedDeltaTime + .000001*(steady_clock::now() - time).count();
				time = steady_clock::now() + milliseconds(interval);
			} while (b);
		}
	}

	void Core::Close() {
		init = false;
		render->Shutdown();
		_DELETE(render);
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
		default:
			ThrowIfFailed(E_INVALIDARG);
			break;
		}
	}
}