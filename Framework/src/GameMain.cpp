#include <GameEngine/GameMain.h>
#include <GameEngine/macros.h>
#include <GameEngine/Device.h>
#include <chrono>
#include <thread>

using namespace std::chrono;

namespace GameEngine {

	GameMain::GameMain(): wnd(nullptr), render(nullptr), init(false), device(nullptr) {}

	GameMain::~GameMain() {}
	bool GameMain::Initialize() {
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
			auto fixedDeltaTime = (long long)(1000. / fps);
			auto time = steady_clock::now();
			auto b = false;
			do {
				b = frame();
				RunScripts(updateScripts);
				std::this_thread::sleep_until(time);
				time = steady_clock::now() + milliseconds(fixedDeltaTime);
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

	void GameMain::AddScript(ScriptBase& script, Stage stage) {
		script.Init(this, &render);
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