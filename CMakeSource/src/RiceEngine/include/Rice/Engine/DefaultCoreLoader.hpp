#include "../stdafx.hpp"

#include "Core.hpp"
#include "Rice/Scene/Scene.hpp"

NSP_TESTS_BEGIN

class DefaultCoreLoader : public Core::Loader {
    String windowCaption;
    pScene scene;

  public:
    DefaultCoreLoader(String windowCaption, pScene scene)
        : windowCaption(windowCaption), scene(scene) {}

    void setupCore(Core::SetupParams &params) override {
        params.window_desc.caption = windowCaption;
    }

    void initCore(Core::InitParams &params) override {}

    void postInitCore(pEngine engine) override { engine->loadScene(scene); }
};

NSP_TESTS_END