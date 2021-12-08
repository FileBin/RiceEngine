#include <GameFramework/Render.h>
#include <GameFramework/stdafx.h>
#include <GameFramework/macros.h>

namespace GameEngine {
	Render::Render() : device(nullptr) {}

	Render::~Render() {}

	void Render::BeginFrame() {
		device->ClearFrame();
	}

	bool Render::Draw() {
		device->Draw();
		return true;
	}

	void Render::EndFrame() {
		device->SwapBuffers();
	}

	void Render::Shutdown() {
		Close();
	}

	void Render::SetDevice(Device* device) {
		this->device = device;
	}
	Vector2 Render::GetViewportSize() {
		auto h = device->GetHWND();
		RECT r;
		GetClientRect(h, &r);
		return Vector2(r.right - r.left, r.bottom - r.top);
	}
	double Render::GetAspectRatio() {
		auto s = GetViewportSize();
		return s.x / s.y;
	}
}
