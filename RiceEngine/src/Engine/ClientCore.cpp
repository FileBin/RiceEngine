#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include "pch.h"
#include <Rice/Engine/ClientCore.hpp>

using namespace std::chrono;
using namespace std::this_thread;

ptr<Rice::ClientScene> castScene(ptr<Rice::SceneBase> scene) {
    auto s = std::dynamic_pointer_cast<Rice::ClientScene>(scene);
    if (!s) {
        THROW_EXCEPTION("ClientCore::loadScene: scene is not ClientScene");
    }
    return s;
}

NSP_ENGINE_BEGIN

ptr<ClientEngine> ClientCore::getClientEngine() {
    return std::dynamic_pointer_cast<ClientEngine>(engine);
}

// runs new instance of Core via core loader
// interface defined by user or default implementation
void ClientCore::runNew(ptr<ClientCore::Loader> core_loader) {
    setCurrentThreadPriority(ThreadPriority::TimeCritical);
    auto core = ptr<ClientCore>(new ClientCore(core_loader));
    core->init();
    core->run();
    core->close();
}

ClientCore::ClientCore(ptr<Loader> core_loader)
    : CoreBase(), loader(core_loader) {}

ClientCore::~ClientCore() {
    if (activeScene) {
        activeScene->close();
    }
    if (loadingScene) {
        if (loadingScene->isLoaded())
            loadingScene->close();
    }
    if (loadingScreenScene) {
        loadingScreenScene->close();
    }
}

// scene loading function in parallel with the main thread
void ClientCore::loadScene(ptr<SceneBase> new_scene) {
    if (loadingScene)
        return;
    loadingScene = castScene(new_scene);
}

// scene checking and loading function
void ClientCore::loadSceneImmediate() {
    if (!loadingScene)
        return;
    if (activeScene) {
        activeScene->close();
    }
    activeScene = loadingScene;
    loadingScene = nullptr;
    activeScene->setup(engine);
    ClientCore::runTask([this]() {
        activeScene->load();
        // activeScene->PostInit();
    });
}
// init core
bool ClientCore::init() {
    auto cli_engine = ClientEngine::create(shared_from_this());
    engine = static_pointer_cast<EngineBase>(cli_engine);

    // TODO AL::Init();
    Log::init();

    SetupParams setupParams;

    loader->setupCore(setupParams);

    wnd = Window::create(setupParams.window_desc);

    if (!wnd) {
        Log::log(Log::Error, "Window could not be created");
        return false;
    }
    graphics_manager = Graphics::GraphicsManager::create();
    graphics_manager->init(wnd);

    InitParams initParams;

    loader->initCore(initParams);

    if (initParams.loading_scene) {
        loadingScreenScene = castScene(initParams.loading_scene);
        loadingScreenScene->setup(engine);
        loadingScreenScene->init();
    }
    // loadingScreenScene->PostInit();

    is_init = true;
    loader->postInitCore(cli_engine);
    return true;
}

// run the core loop
void ClientCore::run() {
    if (is_init) {
        auto fixedDeltaTime = 1000. / update_rate; // calculate fixed delta time
        num interval =
            (num)fixedDeltaTime - 1; // calculate the interval between frames
        auto deltaTime = fixedDeltaTime; // calculate the delta time
        auto begin_time = steady_clock::now();
        time = 0;       // set the time to 0
        auto b = false; // life cycle flag

        time_point start = steady_clock::now(); // start time
        do {                      // loop until the window is not closed
            loadSceneImmediate(); // load scene if it is not loaded yet
            // loader->update();
            // wnd->inputmgr->update(); // update input manager state
            b = runFrame(); // do frame update and draw
            // engine->postUpdate();
            sleep_until(begin_time); // limit framerate
            time = std::chrono::duration<dbl, std::chrono::seconds::period>(
                       steady_clock::now() - start)
                       .count(); // calculate time since frame start
            deltaTime =
                fixedDeltaTime + .000001 * (steady_clock::now() - begin_time)
                                               .count(); // calculate delta time
            begin_time =
                steady_clock::now() +
                milliseconds(interval); // calculate the next frame time
            deltaTime = Math::min(
                deltaTime,
                300.); // limit deltaTime to 300ms to avoid physics stuttering
            this->deltaTime = deltaTime * .001; // divide by 1000 to get seconds
            this->fixedDeltaTime =
                fixedDeltaTime * .001; // divide by 1000 to get seconds
        } while (b);
    }
}

// close the window and clean up everything
void ClientCore::close() {
    loader->onClose(getClientEngine());
    is_init = false;
    if (activeScene)
        activeScene->close();
    if (loadingScene && loadingScene->isLoaded())
        loadingScene->close();
    if (loadingScreenScene)
        loadingScreenScene->close();
    graphics_manager->cleanup();
    wnd->cleanup();
    Log::close();
}

bool ClientCore::runFrame() {
    if (wnd->isExit())
        return false;

    graphics_manager->update();

    if (!activeScene || !activeScene->isLoaded()) {
        if (loadingScreenScene) {
            loadingScreenScene->render();
        } else {
            Log::log(
                Log::Warning,
                "Can't render frame! Loading screen render is not set up!");
        }
    } else {
        activeScene->update();
        activeScene->render();
    }

    return true;
}

NSP_ENGINE_END