#include "../stdafx.hpp"

NSP_ENGINE_BEGIN
class ClientCore;
NSP_ENGINE_END

#pragma once
#include "CoreBase.hpp"
#include "Log.hpp"
#include "Window.hpp"

#include "ClientEngine.hpp"

#include "Rice/Scene/ClientScene.hpp"
#include "Rice/GL/GraphicsManager.hpp"

NSP_ENGINE_BEGIN

class ClientCore : public CoreBase, public enable_ptr<ClientCore> {
  public:
    struct SetupParams {
        DescWindow window_desc;
    };

    struct InitParams {
        ptr<SceneBase> loading_scene;
    };

    class Loader {
      protected:
        friend class ClientCore;
        // executes after window creation before core inits
        virtual void setupCore(SetupParams &setupParams){};
        // executes after all manager inits
        virtual void initCore(InitParams &initParams){};
        // executes after init
        virtual void postInitCore(ptr<ClientEngine> engine){};
        // executes when window closing
        virtual void onClose(ptr<ClientEngine> engine){};
    };
  public:
    static void runNew(ptr<Loader> core_loader);

    void loadScene(ptr<SceneBase> scene) override;

  private:
    ClientCore(ptr<Loader> loader);

  public:
    ~ClientCore();

  private:
    friend class ClientEngine;

    bool init();
    void run();
    void close();

    bool runFrame();

    void loadSceneImmediate();

    ptr<ClientScene> activeScene, loadingScene, loadingScreenScene;

    ptr<ClientEngine> getClientEngine();

    ptr<Window> wnd;
    ptr<Loader> loader;
    ptr<Graphics::GraphicsManager> graphics_manager;
    bool is_init = false;
};

NSP_ENGINE_END
