#include <GameFramework/GameMain.h>
#include <GameFramework/macros.h>
#include <GameFramework/Device.h>
#include <chrono>
#include <thread>

using namespace std::chrono;

namespace GameEngine {

	GameMain::GameMain(): wnd(nullptr), render(nullptr), init(false), device(nullptr) {}
	GameMain::~GameMain() {
		
	}
	bool GameMain::Init() {
		Log::Init();
		wnd = new Window();

		if (!wnd) {
			Log::Err(L"Ќе удалось выделить пам€ть");
			return false;
		}

		DescWindow desc;
		if (!wnd->Create(desc)) {
			Log::Err(L"Ќе удалось создать окно");
			return false;
		}

		device = new Device();
		auto hwnd = wnd->GetHWND();

		if (FAILED(device->Init(hwnd))) {
			Log::Err(L"Can't init device!");
			return false;
		}

		render->SetDevice(device);

		if (!render->Init()) {
			Log::Err(L"Ќе удалось создать рендер");
			return false;
		}

		init = true;
		return true;
	}
	void GameMain::Run() {
		if (init) {
			auto fixedDeltaTime = (long long)(1000. / fps);
			auto time = steady_clock::now();
			do {
				std::this_thread::sleep_until(time);
				time = steady_clock::now() + milliseconds(fixedDeltaTime);
			} while (frame());
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
		// обрабатываем событи€ окна
		wnd->RunEvent();
		// если окно неактивно - завершаем кадр
		if (!wnd->IsActive())
			return true;

		// если окно было закрыто, завершаем работу движка
		if (wnd->IsExit())
			return false;

		// если окно изменило размер
		if (wnd->IsResize()) {
			device->Resize();
			render->Resize();
		}

		render->BeginFrame();
		if (!render->Draw())
			return false;
		render->EndFrame();

		return true;
	}
}