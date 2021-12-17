#pragma once

#include "stdafx.h"
#include "RenderBase.h"
#include "Log.h"
#include "Window.h"
#include "ScriptBase.h"
#include "Stage.h"

namespace Game {

	class ScriptBase;
	class Engine;

	class Core {
	public:
		static void RunNew(ScriptBase* preInitScript);

		void AddScript(ScriptBase* script, Stage s);

		void SetRender(RenderBase* render) { this->render = render; }
		void SetFps(double fps) { this->fps = fps; }

		double GetFixedDeltaTime() { return fixedDeltaTime; }
		double GetDeltaTime() { return deltaTime; }
	private:
		Core();
		~Core();

		bool Initialize();
		void Run();
		void Close();

		bool frame();
		void RunScripts(std::vector<ScriptBase*>& scripts);

		Stage stage;

		Window* wnd = nullptr;
		RenderBase* render = nullptr;
		Device* device = nullptr;
		Engine* engine = nullptr;
		bool init;
		double fps = 60.;
		double fixedDeltaTime, deltaTime;
		std::vector<ScriptBase*> preInitScripts, initScripts, postInitScripts,
			preRenderScripts, renderScripts, postRenderScripts,
			updateScripts;

	};
}
