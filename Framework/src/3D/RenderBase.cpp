#include "pch.h"
#include <GameEngine/RenderBase.h>
#include <GameEngine/stdafx.h>
#include <GameEngine/macros.h>

namespace Game {
	RenderBase::RenderBase() : device(nullptr) {}

	RenderBase::~RenderBase() {}

	void RenderBase::BeginFrame() {
		device->ClearFrame();
	}

	bool RenderBase::Draw() {
		device->Draw();
		return true;
	}

	void RenderBase::EndFrame() {
		device->SwapBuffers();
	}

	void RenderBase::Shutdown() {
		//Close();
	}

	void RenderBase::SetDevice(Device* device) {
		this->device = device;
	}

	Vector2 RenderBase::GetViewportSize() {
		auto h = device->GetHWND();
		RECT r;
		GetClientRect(h, &r);
		return Vector2((double)r.right - r.left, (double)r.bottom - r.top);
	}

	double RenderBase::GetAspectRatio() {
		auto s = GetViewportSize();
		return s.x / s.y;
	}
}
