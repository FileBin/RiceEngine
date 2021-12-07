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
			Log::Err(L"�� ������� �������� ������");
			return false;
		}

		// ������� �������� �������� �� ���������. � ����� �� ������� ������ �� �������� � �����
		DescWindow desc;
		if (!wnd->Create(desc)) {
			Log::Err(L"�� ������� ������� ����");
			return false;
		}

		if (!render->CreateDevice(wnd->GetHWND())) {
			Log::Err(L"�� ������� ������� ������");
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
		// ������������ ������� ����
		wnd->RunEvent();
		// ���� ���� ��������� - ��������� ����
		if (!wnd->IsActive())
			return true;

		// ���� ���� ���� �������, ��������� ������ ������
		if (wnd->IsExit())
			return false;

		// ���� ���� �������� ������
		if (wnd->IsResize()) {}
		render->BeginFrame();
		if (!render->Draw())
			return false;
		render->EndFrame();

		return true;
	}
}