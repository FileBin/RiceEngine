#pragma once

#include "stdafx.h"
#include "RenderBase.h"
#include "Log.h"
#include "Window.h"
#include "ScriptBase.h"

namespace GameEngine {

	class GameMain {
	public:
		enum Stage {
			PreInit = 0,
			Init,
			PostInit,
			PreRender,
			Render,
			PostRender,
			Update,
		};

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
		void RunScripts(List<ScriptBase*>& scripts) {
			for (auto s : scripts) { s->Run(); }
		}

		Window* wnd;
		RenderBase* render;
		Device* device;
		bool init;
		double fps = 60.;
		List<ScriptBase*> preInitScripts, initScripts, postInitScripts,
			preRenderScripts, renderScripts, postRenderScripts,
			updateScripts;

	};
}
