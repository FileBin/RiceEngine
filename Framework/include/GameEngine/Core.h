#pragma once

#include "stdafx.h"
#include "RenderBase.h"
#include "Log.h"
#include "Window.h"
#include "ScriptBase.h"
#include "Stage.h"
#include <functional>
#include <ppltasks.h>

namespace Game {

	class ScriptBase;
	class Engine;

	class Core {
	public:
		static void RunNew(ScriptBase* preInitScript);

		template <class FnT = void(void), class... ArgsT>
		static SmartPtr<std::thread> RunThread(std::function<FnT> func, ArgsT... args) {
			return new std::thread([](std::function<FnT> _Fx, ArgsT... _Ax) {
				try {
					_Fx(_Ax...);
				}
				#include "../src/Util/ExeptionManager.h"
				}, func, args...);
		}

		static void RunTask(std::function<void(void)> func) {
			 concurrency::create_task([func]() {
				try {
					func();
				}
				#include "../src/Util/ExeptionManager.h"
				});
		}

		void AddScript(ScriptBase* script, Stage s);

		void SetRender(RenderBase* render) { this->render = render; }
		void SetFps(double fps) { this->fps = fps; }

		double GetFixedDeltaTime() { return fixedDeltaTime; }
		double GetDeltaTime() { return deltaTime; }
		double GetTime() { return time; }
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
		double fixedDeltaTime, deltaTime, time;
		std::vector<ScriptBase*> preInitScripts, initScripts, postInitScripts,
			preRenderScripts, renderScripts, postRenderScripts,
			updateScripts, closeScripts;

	};
}
