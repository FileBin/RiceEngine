#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Core;
typedef RefPtr<Core> pCore;

NSP_ENGINE_END

#pragma once
//#include "../Misc/LoadingScreenRender.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "ScriptBase.hpp"
#include "../Misc/Stage.hpp"
#include "../Scene/Scene.hpp"
#include "../GL/GraphicsManager.hpp"

NSP_ENGINE_BEGIN

class Core {
private:
    template<typename RetT, typename... ArgsT>
	static RetT executeFuncAndHandleExceptions(std::function<RetT(ArgsT...)>& fn, ArgsT... args) {
		try {
			return fn(args...);
		} catch (Exception& e) {
            Log::log(Log::Error, "Not handled exception occured: {}", String(typeid(e).name()));
			Log::log(Log::Error, "Line {}", std::to_wstring(e.line()));
            Log::log(Log::Error, "File: {}", e.file());
            Log::log(Log::Error, "Message: {}", e.msg());
            Log::log(Log::Error, "AdditionalInfo: {}\n", e.info());
            Log::log(Log::Error, "Stack trace:\n{}", e.stack());
			Log::close();
			throw e;
		} catch (std::exception& e) {
            Log::log(Log::Error, "std::exception occured: {}", String(typeid(e).name()));
            Log::log(Log::Error, "What: {}", String(e.what()));
			Log::close();
			throw e;
		}
	}
public:
	static void runNew(RefPtr<ScriptBase> preInitScript);

	template<class... ArgsT>
	static RefPtr<std::thread> runThread(std::function<void(ArgsT...)> func, ArgsT ... args) {
        return new_ref<std::thread>([](std::function<void(ArgsT...)> _Fx, ArgsT... _Ax) {
            executeFuncAndHandleExceptions<void, ArgsT...>(_Fx, _Ax...);
		}, func, args...);
	}

	/*static void RunTask(std::function<void(void)> func) {
	 concurrency::create_task([func]() {
	 try {
	 func();
	 }
	 #include "../src/Util/ExeptionManager.h"
	 });
	 }*/ //TODO: Make tasks

	void addScript(pScriptBase script, Stage s);

	void loadScene(Scening::pScene _scene);

	double getFixedDeltaTime() { return fixedDeltaTime; }
	double getDeltaTime() { return deltaTime; }
	double getTime() { return time; }
private:
	Core();
	~Core();
	friend class Engine;

	bool init();
	void run();
	void close();

	bool runFrame();
	void runScripts(vec<pScriptBase>& scripts);

	void loadSceneImmediate();

	Stage stage = (Stage)0;
    Scening::pScene activeScene, loadingScene, loadingScreenScene;

	pWindow wnd = nullptr;
	Graphics::pGraphicsManager manager = nullptr;
	Engine* engine = nullptr;
    bool is_init = false;
	double fps = 60.;
	double fixedDeltaTime = 0, deltaTime = 0, time = 0;
	vec<pScriptBase> preInitScripts, initScripts, postInitScripts,
	updateScripts, closeScripts;

};

NSP_ENGINE_END
