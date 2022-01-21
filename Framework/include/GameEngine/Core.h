#pragma once

#include "stdafx.h"
#include "LoadingScreenRender.h"
#include "Log.h"
#include "Window.h"
#include "ScriptBase.h"
#include "Stage.h"
#include "Scene\Scene.h"
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

		void LoadScene(Scene* _scene) {
			activeScene = _scene;
		}
		void SetFps(double _fps) { fps = _fps; }

		double GetFixedDeltaTime() { return fixedDeltaTime; }
		double GetDeltaTime() { return deltaTime; }
		double GetTime() { return time; }
	private:
		Core();
		~Core();
		friend class Engine;

		bool Init();
		void Run();
		void Close();

		bool RunFrame();
		void RunScripts(std::vector<ScriptBase*>& scripts);

		Stage stage = (Stage)0;
		SmartPtr<Scene> activeScene;

		Window* wnd = nullptr;
		LoadingScreenRenderBase* render = nullptr;
		Device* device = nullptr;
		Engine* engine = nullptr;
		bool init = false;
		double fps = 60.;
		double fixedDeltaTime = 0, deltaTime = 0, time = 0;
		std::vector<ScriptBase*> preInitScripts, initScripts, postInitScripts,
			updateScripts, closeScripts;

	};
}
