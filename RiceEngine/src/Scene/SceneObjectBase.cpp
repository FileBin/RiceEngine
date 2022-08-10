#include "Rice/Scene/SceneObjectBase.hpp"
#include "pch.h"

#include <Rice/Scene/ClientScene.hpp>
#include <Rice/Scene/SceneBase.hpp>

NSP_ENGINE_BEGIN

SceneObjectBase::SceneObjectBase() {}

SceneObjectBase::~SceneObjectBase() {}

void SceneObjectBase::enable() { flags |= Flags::NEED_ENABLE; }

void SceneObjectBase::disable() { flags |= Flags::NEED_DISABLE; }

void SceneObjectBase::update() {
    if (!isEnabled())
        return;
    onUpdate();
}

void SceneObjectBase::forceEnable() {
    if (isEnabled() || !canEnable())
        return;
    flags |= Flags::ENABLED;
    onEnable();
}

void SceneObjectBase::forceDisable() {
    if (!isEnabled())
        return;
    flags &= ~Flags::ENABLED;
    onDisable();
}

void SceneObjectBase::preUpdate() {
    auto en = flags & Flags::NEED_ENABLE ? true : false;
    auto dis = flags & Flags::NEED_DISABLE ? true : false;

    en &= canEnable();

    if (en ^ dis) {
        if (en) {
            forceEnable();
        } else if (dis) {
            forceDisable();
        }
    }
    flags &= ~(Flags::NEED_ENABLE | Flags::NEED_DISABLE);

    onPreUpdate();
}

bool SceneObjectBase::canEnable() {
    auto p = getBaseParent();
    if(!p)
        return true;
    if (!p->isEnabled())
        return false;
    return p->canEnable();
}

bool SceneObjectBase::isEnabled() { return flags & Flags::ENABLED; }

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