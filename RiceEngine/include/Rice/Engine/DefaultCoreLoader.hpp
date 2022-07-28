#include "../stdafx.hpp"

#include "Core.hpp"
#include "Rice/Scene/Scene.hpp"
#include "Rice/namespaces.h"

NSP_ENGINE_BEGIN

class DefaultCoreLoader : public Core::Loader {
    String windowCaption;
    pScene scene;

  public:
    DefaultCoreLoader(String windowCaption, pScene scene)
        : windowCaption(windowCaption), scene(scene) {}

    void setupCore(Core::SetupParams &params) override {
        params.window_desc.caption = windowCaption;
    }

    void initCore(Core::InitParams &params) override {
        params.loading_scene = nullptr;
    }

    void postInitCore(pEngine engine) override { engine->loadScene(scene); }
};

NSP_ENGINE_END