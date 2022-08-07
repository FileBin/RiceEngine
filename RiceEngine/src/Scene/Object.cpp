#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/PackableComponent.hpp"
#include "Rice/Scene/Scene.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/Util/String.hpp"
#include "Rice/defines.h"

NSP_ENGINE_BEGIN

Object::Object(ptr<Scene> scene, String name, UUID uuid)
    : scene(scene), name(name), selfUUID(uuid) {}

void Object::init(ptr<Object> parent) {
    this->parent = parent;
    // subscribe to parent state changed event to update object enabled state
    parent->events.stateChanged->subscribe(
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

void Object::enable() {
    // if component is already enabled, return
    if (enableRegistration.isRegistered() || isEnabled())
        return;
    // subscribe to scene process event to update object state
    getScene()->events.processEvents->subscribe(enableRegistration, [this]() {
        forceEnable();
        enableRegistration.cleanup(); // call only once
    });
}

void Object::disable() {
    // if component is already disabled, return
    if (disableRegistration.isRegistered() || !isEnabled())
        return;
    // subscribe to scene process event to update component state
    getScene()->events.processEvents->subscribe(disableRegistration, [this]() {
        forceDisable();
        disableRegistration.cleanup(); // call only once
    });
}

void Object::forceEnable() {
    if (canUpdate())
        onEnable();
    // subscribe to parent update event to update object
    if (!updateRegistration.isRegistered())
        getParent()->events.update->subscribe(updateRegistration,
                                              [this]() { update(); });
}

void Object::forceDisable() {
    onDisable();
    // unsubscribe from object update event
    updateRegistration.cleanup();
}

bool Object::canUpdate() {
    auto p = getParent();
    if (p)
        return p->isEnabled();
    return isEnabled();
}

// update all components and child objects
void Object::update() { events.update->invoke(); }

// notify all components and child objects that object is enabled
void Object::onEnable() { events.stateChanged->invoke(true); }

// notify all components and child objects that object is disabled
void Object::onDisable() { events.stateChanged->invoke(false); }

bool Object::isEnabled() { return updateRegistration.isRegistered(); }

ptr<Object> Object::createEmpty(String name) {
    ptr<Object> obj{new Object(getScene(), name, getScene()->getNextUUID())};
    obj->init(shared_from_this());
    children.push_back(obj);
    return obj;
}

ptr<Object> Object::createEnabled(String name) {
    auto o = createEmpty(name);
    o->enable();
    return o;
}

ptr<Scene> Object::getScene() {
    auto scene_lock = scene.lock();
    if (!scene_lock)
        THROW_NULL_PTR_EXCEPTION(scene_lock.get());
    return scene_lock;
}

ptr<Object> Object::getParent() {
    auto parent_lock = parent.lock();
    if (!parent_lock)
        THROW_NULL_PTR_EXCEPTION(parent_lock.get());
    return parent_lock;
}

void Object::addComponent(ptr<Components::PackableComponent> component) {
    components.registerPtr(component);
    component->init(shared_from_this());
    component->enable();
}

ObjectData Object::pack() {
    auto parent_lock = parent.lock();

    ObjectData data;
    data.enabled = isEnabled();
    data.selfUUID = selfUUID;
    if (parent_lock)
        data.parentUUID = parent_lock->selfUUID;
    else
        data.parentUUID = 0;

    uint n = children.size();
    data.childrenUUID.resize(n);
    for (uint i = 0; i < n; i++) {
        data.childrenUUID[i] = children[i]->selfUUID;
    }

    auto components_vec = components.getCollection();

    n = components_vec.size();

    ByteStream stream(data.componentsData);

    for (uint i = 0; i < n; i++) {
        stream.write(components_vec[i]->pack());
    }

    return data;
}

ptr<Object>
ObjectData::unpack(ptr<Scene> scene,
                   std::function<ObjectData(UUID)> getRelativesData) {
    auto parent = scene->getObject(parentUUID);

    if (!parent) {
        parent = getRelativesData(parentUUID).unpack(scene, getRelativesData);
    }

    auto self_obj = unpack(parent, getRelativesData);

    return self_obj;
}

ptr<Object>
ObjectData::unpack(ptr<Object> parent,
                   std::function<ObjectData(UUID)> getRelativesData) {
    ptr<Object> inst{new Object(parent->getScene(), name, selfUUID)};
    inst->init(parent);

    if (enabled)
        inst->enable();
    else
        inst->disable();

    ByteStream stream(componentsData);
    while (!stream.empty()) {
        auto component = Components::PackableComponent::unpack(stream);
        inst->addComponent(component);
    }

    for (auto childUUID : childrenUUID) {
        auto data = getRelativesData(childUUID);
        data.unpack(inst, getRelativesData);
    }

    parent->children.push_back(inst);

    return inst;
}

NSP_ENGINE_END