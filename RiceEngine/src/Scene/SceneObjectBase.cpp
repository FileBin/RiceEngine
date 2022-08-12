#include "Rice/Scene/SceneObjectBase.hpp"
#include "pch.h"

#include <Rice/Scene/ClientScene.hpp>
#include <Rice/Scene/SceneBase.hpp>

NSP_ENGINE_BEGIN

SceneObjectBase::SceneObjectBase() {}

SceneObjectBase::~SceneObjectBase() {}

void SceneObjectBase::enable() { flags |= Flags::NEED_ENABLE; }

void SceneObjectBase::disable() { flags |= Flags::NEED_DISABLE; }

void SceneObjectBase::destroy() { flags |= Flags::NEED_DESTROY; }

void SceneObjectBase::update() {
    if (flags & Flags::DESTROYED)
        return;
    if (!isActive())
        return;
    onUpdate();
}

void SceneObjectBase::forceEnable() {
    if (flags & Flags::DESTROYED)
        return;
    if (isActive() || !canEnable())
        return;
    flags |= Flags::ENABLING;
    onEnable();
    flags &= ~Flags::ENABLING;
    flags |= Flags::ACTIVE;
}

void SceneObjectBase::forceDisable() {
    if (flags & Flags::DESTROYED)
        return;
    if (!isActive())
        return;
    onDisable();
    flags &= ~Flags::ACTIVE;
}

void SceneObjectBase::forceDestroy() {
    if (flags & Flags::DESTROYED)
        return;
    onDestroy();
    flags = Flags::DESTROYED;
}

void SceneObjectBase::preUpdate() {
    if (flags & Flags::DESTROYED)
        return;
    auto en = flags & Flags::NEED_ENABLE ? true : false;
    auto dis = flags & Flags::NEED_DISABLE ? true : false;
    auto des = flags & Flags::NEED_DESTROY ? true : false;

    if (des) {
        forceDestroy();
        return;
    }

    en &= canEnable();

    if (dis) {
        forceDisable();
    } else if (en) {
        forceEnable();
    }

    flags &= ~(Flags::NEED_ENABLE | Flags::NEED_DISABLE);

    onPreUpdate();
}

bool SceneObjectBase::canEnable() {
    auto p = getBaseParent();
    if (!p)
        return true;
    if (p->flags & Flags::ENABLING || p->flags & Flags::ACTIVE)
        return p->canEnable();
    return false;
}

bool SceneObjectBase::isActive() { return flags & Flags::ACTIVE; }
bool SceneObjectBase::isEnabled() {
    return (flags & Flags::NEED_ENABLE) || (flags & Flags::ACTIVE);
}

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
                        "config? \n Try to use #ifndef SERVER");
    }
    return cli_scene;
}

ptr<ClientEngine> SceneObjectBase::getClientEngine() {
    auto cli_engine = std::dynamic_pointer_cast<ClientEngine>(getEngine());
    if (!cli_engine) {
        THROW_EXCEPTION("Engine is not a ClientEngine! Maybe you use server "
                        "config? \n Try to use #ifndef SERVER");
    }
    return cli_engine;
}
NSP_ENGINE_END