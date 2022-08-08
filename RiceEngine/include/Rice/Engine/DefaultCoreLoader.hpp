#pragma once
#undef RICE_SOURCE
#include "Rice/Engine.hpp"
#include "Rice/Engine/Core.hpp"

NSP_ENGINE_BEGIN

class DefaultCoreLoader : public Core::Loader {
    String windowCaption;
    ptr<Scene> scene;

  public:
    DefaultCoreLoader(String windowCaption, ptr<Scene> scene)
        : windowCaption(windowCaption), scene(scene) {}

    void setupCore(Core::SetupParams &params) override {
        params.window_desc.caption = windowCaption;
    }

    void initCore(Core::InitParams &params) override {
        params.loading_scene = nullptr;
    }

    void postInitCore(ptr<Engine> engine) override { engine->loadScene(scene); }
};

NSP_ENGINE_END