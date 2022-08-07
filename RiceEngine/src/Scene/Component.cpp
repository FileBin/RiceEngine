#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/Scene.hpp"
#include "Rice/Util/Exceptions.hpp"
#include "Rice/defines.h"

NSP_COMPONENTS_BEGIN

void Component::onEnable() {}
void Component::onDisable() {}
void Component::onUpdate() {}

void Component::init(ptr<Object> object) {
    this->object = object;
    // subscribe to object state changed event to update component enabled state
    object->events.stateChanged->subscribe(
        stateChangedRegistration, [this](bool state) {
            if (updateRegistration.isRegistered()) {
                if (state) {
                    onEnable();
                } else {
                    onDisable();
                }
            }
        });
}

Component::~Component() {}

void Component::forceEnable() {
    if (getObject()->canUpdate())
        onEnable();
    // subscribe to object update event to update component
    if (!updateRegistration.isRegistered())
        getObject()->events.update->subscribe(updateRegistration,
                                              [this]() { onUpdate(); });
}
void Component::forceDisable() {
    onDisable();
    // unsubscribe from object update event
    updateRegistration.cleanup();
}

void Component::enable() {
    // if component is already enabled, return
    if (enableRegistration.isRegistered() || isEnabled())
        return;
    // subscribe to scene process event to update component state
    getScene()->events.processEvents->subscribe(enableRegistration, [this]() {
        forceEnable();
        enableRegistration.cleanup(); // call only once
    });
}

void Component::disable() {
    // if component is already disabled, return
    if (disableRegistration.isRegistered() || !isEnabled())
        return;
    // subscribe to scene process event to update component state
    getScene()->events.processEvents->subscribe(disableRegistration, [this]() {
        forceDisable();
        disableRegistration.cleanup(); // call only once
    });
}

bool Component::isEnabled() { return updateRegistration.isRegistered(); }

bool Component::isSceneLoaded() { return getScene()->isLoaded(); }

ptr<Scene> Component::getScene() { return getObject()->getScene(); }

ptr<Engine> Component::getEngine() { return getScene()->getEngine(); }

ptr<SceneRender> Component::getSceneRender() {
    return getScene()->getSceneRender();
}

ptr<Object> Component::getObject() {
    auto lock = object.lock();
    if (!lock)
        THROW_NULL_PTR_EXCEPTION("Object is null");
    return lock;
}

NSP_COMPONENTS_END