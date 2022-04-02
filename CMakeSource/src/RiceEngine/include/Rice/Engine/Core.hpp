#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Core;
typedef SmartPtr<Core> pCore;

NSP_ENGINE_END

#pragma once
#include "../Misc/LoadingScreenRender.hpp"
#include "../Log.hpp"
#include "Window.hpp"
#include "ScriptBase.hpp"
#include "Stage.hpp"
#include "Scene/Scene.hpp"
#include "../GL/GraphicsManager.hpp"

NSP_ENGINE_BEGIN

class Core {
public:
	static void RunNew(ScriptBase* preInitScript);

	template<class FnT = void(void), class ... ArgsT>
	static SmartPtr<_STD thread> RunThread(_STD function<FnT> func,
			ArgsT ... args) {
		return new _STD thread([](_STD function<FnT> _Fx, ArgsT... _Ax) {
					try {
						_Fx(_Ax...);
					}
#include "../src/Util/ExeptionManager.h"
				}, func, args...);
	}

	/*static void RunTask(std::function<void(void)> func) {
	 concurrency::create_task([func]() {
	 try {
	 func();
	 }
	 #include "../src/Util/ExeptionManager.h"
	 });
	 }*/

	void AddScript(ScriptBase* script, Stage s);

	void LoadScene(Scening::pScene _scene);

	double GetFixedDeltaTime() {return fixedDeltaTime;}
	double GetDeltaTime() {return deltaTime;}
	double GetTime() {return time;}
private:
	Core();
	~Core();
	friend class Engine;

	bool Init();
	void Run();
	void Close();

	bool RunFrame();
	void RunScripts(std::vector<ScriptBase*>& scripts);

	void LoadSceneImmediate();

	Stage stage = (Stage)0;
	Scening::pScene activeScene, loadScene, loadingScreenScene;

	pWindow wnd = nullptr;
	Graphics::pGraphicsManager manager = nullptr;
	Engine* engine = nullptr;
	bool init = false;
	double fps = 60.;
	double fixedDeltaTime = 0, deltaTime = 0, time = 0;
	std::vector<ScriptBase*> preInitScripts, initScripts, postInitScripts,
	updateScripts, closeScripts;

};

NSP_ENGINE_END
