#include "../stdafx.hpp"

NSP_ENGINE_BEGIN
class Core;
NSP_ENGINE_END

#pragma once
#include "Log.hpp"
#include "Window.hpp"

#include "Engine.hpp"

#include "../Scene/Scene.hpp"
#include "Rice/GL/GraphicsManager.hpp"

NSP_ENGINE_BEGIN

class Core : public enable_ptr<Core> {
  public:
    struct SetupParams {
        DescWindow window_desc;
    };

    struct InitParams {
        ptr<Scene> loading_scene;
    };

    class Loader {
      protected:
        friend class Core;
        // executes after window creation before core inits
        virtual void setupCore(SetupParams &setupParams){};
        // executes after all manager inits
        virtual void initCore(InitParams &initParams){};
        // executes after init
        virtual void postInitCore(ptr<Engine> engine){};
        // executes when window closing
        virtual void onClose(ptr<Engine> engine){};
    };

  private:
    template <typename RetT, typename... ArgsT>
    static RetT
    executeFuncAndHandleExceptions(std::function<RetT(ArgsT...)> &fn,
                                   ArgsT... args) {
        try {
            return fn(args...);
        } catch (Util::Exception &e) {
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
    static void runNew(ptr<Loader> core_loader);

    template <class... ArgsT>
    static ptr<std::thread> runThread(std::function<void(ArgsT...)> func,
                                      ArgsT... args) {
        return ptr<std::thread>(new std::thread(
            [](std::function<void(ArgsT...)> _Fx, ArgsT... _Ax) {
                executeFuncAndHandleExceptions<void, ArgsT...>(_Fx, _Ax...);
            },
            func, args...));
    }

    static void runTask(std::function<void(void)> func) {
        // TODO make tasks
        func(); // temporary replacement
    }

    void loadScene(ptr<Scene> scene);

    dbl getFixedDeltaTime() { return fixedDeltaTime; }
    dbl getDeltaTime() { return deltaTime; }
    dbl getTime() { return time; }

  private:
    Core(ptr<Loader> loader);

  public:
    ~Core();

  private:
    friend class Engine;

    bool init();
    void run();
    void close();

    bool runFrame();

    void loadSceneImmediate();

    ptr<Scene> activeScene, loadingScene, loadingScreenScene;

    ptr<Window> wnd;
    ptr<Loader> loader;
    ptr<Graphics::GraphicsManager> graphics_manager;
    ptr<Engine> engine;
    bool is_init = false;
    dbl fps = 60.;
    dbl fixedDeltaTime = 0, deltaTime = 0, time = 0;
};

NSP_ENGINE_END
