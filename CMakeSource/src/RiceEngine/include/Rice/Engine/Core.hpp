#include "../stdafx.hpp"
#include "BetterCpp/Objects/BasePtr.hpp"

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

class Core : public EnableThisRefPtr<Core> {
  public:
    struct SetupParams {
        DescWindow window_desc;
    };

    struct InitParams {
        pScene loading_scene;
    };

    class Loader {
      protected:
        friend class Core;
        // executes after window creation before core inits
        virtual void setupCore(SetupParams &setupParams){};
        // executes after all manager inits
        virtual void initCore(InitParams &initParams){};
        // executes after init
        virtual void postInitCore(pEngine engine){};
        // executes when window closing
        virtual void onClose(pEngine engine){};
    };

    typedef RefPtr<Loader> pLoader;

  private:
    template <typename RetT, typename... ArgsT>
    static RetT
    executeFuncAndHandleExceptions(std::function<RetT(ArgsT...)> &fn,
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

  public:
    ~Core();

  private:
    friend class Engine;

    bool init();
    void run();
    void close();

    bool runFrame();

    void loadSceneImmediate();

    pScene activeScene, loadingScene, loadingScreenScene;

    pWindow wnd;
    pLoader loader;
    Graphics::pGraphicsManager graphics_manager;
    pEngine engine;
    bool is_init = false;
    dbl fps = 60.;
    dbl fixedDeltaTime = 0, deltaTime = 0, time = 0;
};

NSP_ENGINE_END
