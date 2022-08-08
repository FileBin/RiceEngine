#include "pch.h"

#include <Rice/Scene/ClientScene.hpp>
#include <Rice/Scene/SceneBase.hpp>

NSP_ENGINE_BEGIN

SceneObjectBase::SceneObjectBase() {}

SceneObjectBase::~SceneObjectBase() {}

ptr<SceneBase> SceneObjectBase::getScene() {
    auto s = scene.lock();
    if (!s) {
        THROW_NULL_PTR_EXCEPTION(s.get());
    }
    return s;
}

ptr<EngineBase> SceneObjectBase::getEngine() { return getScene()->getEngine(); }

ptr<SceneRender> SceneObjectBase::getSceneRender() {
    return getClientScene()->getSceneRender();
}

UUID SceneObjectBase::getUUID() { return uuid; }

ptr<ClientScene> SceneObjectBase::getClientScene() {
    auto cli_scene = std::dynamic_pointer_cast<ClientScene>(getScene());
    if (!cli_scene) {
        THROW_EXCEPTION("Scene is not a ClientScene! Maybe you use server "
                        "config? \n Try to use #ifndef SERVER_CONFIG");
    }
    return cli_scene;
}

ptr<ClientEngine> SceneObjectBase::getClientEngine() {
    auto cli_engine = std::dynamic_pointer_cast<ClientEngine>(getEngine());
    if (!cli_engine) {
        THROW_EXCEPTION("Engine is not a ClientEngine! Maybe you use server "
                        "config? \n Try to use #ifndef SERVER_CONFIG");
    }
    return cli_engine;
}

NSP_ENGINE_END