#include "Rice/namespaces.h"
#include "pch.h"
#include <Rice/Engine/Core.hpp>

using namespace std::chrono;
using namespace std::this_thread;

NSP_ENGINE_BEGIN

void Core::runNew(pLoader core_loader) {
  Core core(core_loader);
  core.init();
  core.run();
  core.close();
}

Core::Core(pLoader core_loader) : loader(core_loader) {}

Core::~Core() {}

void Core::loadScene(pScene new_scene) {
  if (loadingScene.isNotNull())
    return;
  loadingScene = new_scene;
}

void Core::loadSceneImmediate() {
  if (loadingScene.isNull())
    return;
  if (activeScene.isNotNull()) {
    activeScene->close();
  }
  activeScene = loadingScene;
  loadingScene = nullptr;
  activeScene->setup(engine);
  Core::runTask([this]() {
    activeScene->init();
    // activeScene->PostInit();
  });
}

bool Core::init() {
  engine = new_ref<Engine>(*this);

  // TODO AL::Init();
  Log::init();
  wnd = new_ref<Window>();

  if (wnd.isNull()) {
    Log::log(Log::Error, L"Window could not be constructed");
    return false;
  }

  loader->setupCore();

  if (!wnd->create(desc)) {
    Log::log(Log::Error, L"Window could not be created");
    return false;
  }

  graphics_manager->init(wnd);

  loader->onInit();

  loadingScreenScene->setup(engine);
  loadingScreenScene->init();
  // loadingScreenScene->PostInit();

  is_init = true;
  loader->postInit();
  return true;
}
void Core::run() {
  if (is_init) {
    auto fixedDeltaTime = 1000. / fps;
    auto interval = (long long)fixedDeltaTime - 1;
    auto deltaTime = fixedDeltaTime;
    auto _time = steady_clock::now();
    time = 0;
    auto b = false;

    time_point start = steady_clock::now();
    do {
      loadSceneImmediate();
      // loader->update();
      wnd->inputmgr->update();
      b = runFrame();
      engine->postUpdate();

      sleep_until(_time);
      time = (steady_clock::now() - start).count() * .000001 * .001;
      deltaTime =
          fixedDeltaTime + .000001 * (steady_clock::now() - _time).count();
      _time = steady_clock::now() + milliseconds(interval);
      deltaTime = Math::min(deltaTime, 300.);
      this->deltaTime = deltaTime * .001;
      this->fixedDeltaTime = fixedDeltaTime * .001;
    } while (b);
  }
}

void Core::close() {
  loader->onClose();
  is_init = false;
  loadingScreenScene->close();
  loadingScreenScene.release();
  wnd->cleanup();
  Log::close();
}

bool Core::runFrame() {
  wnd->update();

  wnd->inputmgr->setActive(wnd->isActive());

  if (wnd->isExit())
    return false;

  if (wnd->isResize()) {
    // device->Resize();
    // loadingScreenScene->Resize();
    // if (!activeScene.IsNull()) {
    //   activeScene->Resize();
    // }
  }
  if (activeScene.isNull() || !activeScene->isLoaded()) {
    loadingScreenScene->render();
  } else {
    activeScene->render();
  }

  return true;
}

NSP_ENGINE_END