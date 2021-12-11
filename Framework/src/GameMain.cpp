#include <GameEngine/GameMain.h>
#include <GameEngine/macros.h>
#include <GameEngine/Device.h>
#include <GameEngine/ScriptBase.h>
#include <GameEngine\Engine.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::this_thread;

namespace GameEngine {

	GameMain::GameMain() {}

	GameMain::~GameMain() {}
	bool GameMain::Initialize() {
		engine = new Engine(&device, this);
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

		device = new Device();
		auto hwnd = wnd->GetHWND();
		RunScripts(initScripts);

		device->Initialize(hwnd);
		render->SetDevice(device);

		if (!render->Init()) {
			Log::Err(L"Не удалось создать рендер");
			return false;
		}
		init = true;
		RunScripts(postInitScripts);
		return true;
	}
	void GameMain::Run() {
		if (init) {
			auto fixedDeltaTime = 1000. / fps;
			auto interval = (long long)fixedDeltaTime;
			auto deltaTime = fixedDeltaTime;
			auto time = steady_clock::now();
			auto b = false;
			do {
				b = frame();
				RunScripts(updateScripts);
				engine->PostUpdate();

				sleep_until(time);
				deltaTime = fixedDeltaTime + .000001*(steady_clock::now() - time).count();
				time = steady_clock::now() + milliseconds(interval);
			} while (b);
		}
	}

	void GameMain::Close() {
		init = false;
		render->Shutdown();
		_DELETE(render);
		_CLOSE(wnd);
		Log::Close();
	}

	bool GameMain::frame() {
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

		RunScripts(preRenderScripts);
		render->BeginFrame();
		RunScripts(renderScripts);
		if (!render->Draw())
			return false;
		render->EndFrame();
		RunScripts(postRenderScripts);
		return true;
	}

	void GameMain::RunScripts(List<ScriptBase*>& scripts){
		for (auto s : scripts) { s->Run(); }
	}


	void GameMain::AddScript(ScriptBase& script, Stage stage) {
		script.Init(&engine, &render);
		script.Initialize();
		switch (stage) {
		case Stage::PreInit:
			preInitScripts.push_back(&script);
			break;
		case Stage::Init:
			initScripts.push_back(&script);
			break;
		case Stage::PostInit:
			postInitScripts.push_back(&script);
			break;
		case Stage::PreRender:
			preRenderScripts.push_back(&script);
			break;
		case Stage::Render:
			renderScripts.push_back(&script);
			break;
		case Stage::PostRender:
			postRenderScripts.push_back(&script);
			break;
		case Stage::Update:
			updateScripts.push_back(&script);
			break;
		default:
			ThrowIfFailed(E_INVALIDARG);
			break;
		}
	}
}