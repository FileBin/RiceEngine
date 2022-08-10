#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/defines.h"
#include "pch.h"

#include <Rice/Scene/Component.hpp>
#include <Rice/Scene/Object.hpp>

NSP_COMPONENTS_BEGIN

void Component::onPreUpdate() {}
void Component::onEnable() {}
void Component::onDisable() {}
void Component::onUpdate() {}

void Component::init(ptr<Object> object) { this->object = object; }

Component::~Component() {}

ptr<SceneObjectBase> Component::getBaseParent() {
    return std::static_pointer_cast<SceneObjectBase>(object.lock());
}

bool Component::isSceneLoaded() { return getScene()->isLoaded(); }

ptr<Object> Component::getObject() {
    auto lock = object.lock();
    if (!lock)
        THROW_NULL_PTR_EXCEPTION("Object is null");
    return lock;
}

NSP_COMPONENTS_END