#include "GameMain.h"
#include "macros.h"

namespace GameFramework {

	GameMain::GameMain(): wnd(nullptr), render(nullptr), init(false) {}
	GameMain::~GameMain() {
		
	}
	bool GameMain::Init() {
		Log::Init();
		wnd = new Window();

		if (!wnd) {
			Log::Err(L"Ќе удалось выделить пам€ть");
			return false;
		}

		// —оздаем значени€ настроек по умолчанию. ¬ одном из будущих уроков мы вернемс€ к этому
		DescWindow desc;
		if (!wnd->Create(desc)) {
			Log::Err(L"Ќе удалось создать окно");
			return false;
		}

		if (!render->CreateDevice(wnd->GetHWND())) {
			Log::Err(L"Ќе удалось создать рендер");
			return false;
		}

		init = true;
		return true;
	}
	void GameMain::Run() {
		if (init)
			while (frame());
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
		if (wnd->IsResize()) {}
		render->BeginFrame();
		if (!render->Draw())
			return false;
		render->EndFrame();

		return true;
	}
}