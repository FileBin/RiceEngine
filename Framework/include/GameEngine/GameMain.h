#pragma once

#include "stdafx.h"
#include "RenderBase.h"
#include "Log.h"
#include "Window.h"
#include "ScriptBase.h"
#include "Stage.h"

namespace GameEngine {

	class ScriptBase;
	class Engine;

	class GameMain {
	public:


		GameMain();
		~GameMain();

		bool Initialize();
		void Run();
		void Close();

		void SetRender(RenderBase* render) { this->render = render; }
		void SetFps(double fps) { this->fps = fps; }

		void AddScript(ScriptBase& script, Stage s = Stage::Update);

	private:
		bool frame();
		void RunScripts(List<ScriptBase*>& scripts);

		Window* wnd = nullptr;
		RenderBase* render = nullptr;
		Device* device = nullptr;
		Engine* engine = nullptr;
		bool init;
		double fps = 60.;
		List<ScriptBase*> preInitScripts, initScripts, postInitScripts,
			preRenderScripts, renderScripts, postRenderScripts,
			updateScripts;

	};
}
