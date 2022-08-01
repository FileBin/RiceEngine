﻿#include "Rice/defines.h"
#include <Rice/Engine/Core.hpp>

using namespace std::chrono;
using namespace std::this_thread;

NSP_ENGINE_BEGIN

// runs new instance of Core via core loader
// interface defined by user or default implementation
void Core::runNew(ptr<Loader> core_loader) {
    auto core = ptr<Core>(new Core(core_loader));
    core->init();
    core->run();
    core->close();
}

Core::Core(ptr<Loader> core_loader) : loader(core_loader) {}

Core::~Core() {}

// scene loading function in parallel with the main thread
void Core::loadScene(ptr<Scene> new_scene) {
    if (loadingScene)
        return;
    loadingScene = new_scene;
}

// scene checking and loading function
void Core::loadSceneImmediate() {
    if (!loadingScene)
        return;
    if (activeScene) {
        activeScene->close();
    }
    activeScene = loadingScene;
    loadingScene = nullptr;
    activeScene->setup(engine);
    Core::runTask([this]() {
        activeScene->load();
        // activeScene->PostInit();
    });
}
// init core
bool Core::init() {
    engine = Engine::create(shared_from_this());

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
        loadingScreenScene = initParams.loading_scene;
        loadingScreenScene->setup(engine);
        loadingScreenScene->init();
    }
    // loadingScreenScene->PostInit();

    is_init = true;
    loader->postInitCore(engine);
    return true;
}

// run the core loop
void Core::run() {
    if (is_init) {
        auto fixedDeltaTime = 1000. / fps; // calculate fixed delta time
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
            wnd->inputmgr->update(); // update input manager state
            b = runFrame();          // do frame update and draw
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
void Core::close() {
    loader->onClose(engine);
    is_init = false;
    if (loadingScreenScene)
        loadingScreenScene->close();
    wnd->cleanup();
    Log::close();
}

bool Core::runFrame() {
    wnd->update();

    wnd->inputmgr->setActive(true);
    wnd->inputmgr->update();

    if (wnd->isExit())
        return false;

    if (wnd->isResize()) {
        // device->Resize();
        // loadingScreenScene->Resize();
        // if (!activeScene.IsNull()) {
        //   activeScene->Resize();
        // }
    }
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