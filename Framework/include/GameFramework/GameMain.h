#pragma once

#include "stdafx.h"
#include "Render.h"
#include "Log.h"
#include "Window.h"

namespace GameEngine {

	class GameMain {
	public:
		GameMain();
		~GameMain();

		bool Init();
		void Run();
		void Close();

		void SetRender(Render* render) { this->render = render; }
		void SetFps(double fps) { this->fps = fps; }

	private:
		bool frame();

		Window* wnd;
		Render* render;
		Device* device;
		bool init;
		double fps = 60.;
	};
}
