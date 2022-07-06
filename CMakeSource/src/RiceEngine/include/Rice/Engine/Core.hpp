#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Core;
typedef RefPtr<Core> pCore;

NSP_ENGINE_END

#pragma once
#include "Log.hpp"
#include "Window.hpp"

#include "Engine.hpp"

#include "../GL/GraphicsManager.hpp"
#include "../Scene/Scene.hpp"

NSP_ENGINE_BEGIN

class Core {
public:
  class Loader {
  protected:
    friend class Core;
    virtual void setupCore();
    virtual void onInit();
	virtual void postInit();
	virtual void onClose();
  };

  typedef RefPtr<Loader> pLoader;

private:
  template <typename RetT, typename... ArgsT>
  static RetT executeFuncAndHandleExceptions(std::function<RetT(ArgsT...)> &fn,
                                             ArgsT... args) {
    try {
      return fn(args...);
    } catch (Exception &e) {
      Log::log(Log::Error, "Not handled exception occured: {}",
               String(typeid(e).name()));
      Log::log(Log::Error, "Line {}", std::to_wstring(e.line()));
      Log::log(Log::Error, "File: {}", e.file());
      Log::log(Log::Error, "Message: {}", e.msg());
      Log::log(Log::Error, "AdditionalInfo: {}\n", e.info());
      Log::log(Log::Error, "Stack trace:\n{}", e.stack());
      Log::close();
      throw e;
    } catch (std::exception &e) {
      Log::log(Log::Error, "std::exception occured: {}",
               String(typeid(e).name()));
      Log::log(Log::Error, "What: {}", String(e.what()));
      Log::close();
      throw e;
    }
  }

public:
  static void runNew(pLoader core_loader);

  template <class... ArgsT>
  static RefPtr<std::thread> runThread(std::function<void(ArgsT...)> func,
                                       ArgsT... args) {
    return new_ref<std::thread>(
        [](std::function<void(ArgsT...)> _Fx, ArgsT... _Ax) {
          executeFuncAndHandleExceptions<void, ArgsT...>(_Fx, _Ax...);
        },
        func, args...);
  }

  static void runTask(std::function<void(void)> func) {
    // TODO make tasks
  }

  void loadScene(pScene scene);

  dbl getFixedDeltaTime() { return fixedDeltaTime; }
  dbl getDeltaTime() { return deltaTime; }
  dbl getTime() { return time; }

private:
  Core(pLoader loader);
  ~Core();
  friend class Engine;

  bool init();
  void run();
  void close();

  bool runFrame();

  void loadSceneImmediate();

  pScene activeScene, loadingScene, loadingScreenScene;

  DescWindow desc;

  pWindow wnd;
  pLoader loader;
  Graphics::pGraphicsManager graphics_manager;
  pEngine engine;
  bool is_init = false;
  dbl fps = 60.;
  dbl fixedDeltaTime = 0, deltaTime = 0, time = 0;
};

NSP_ENGINE_END
