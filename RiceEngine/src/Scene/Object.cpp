#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/PackableComponent.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/Util/String.hpp"
#include "Rice/defines.h"
#include <memory>

NSP_ENGINE_BEGIN

Object::Object(String name) : name(name) {}

void Object::init(ptr<Object> parent) { this->parent = parent; }

void Object::onEnable() {
    auto coll = components.getCollection();
    for (auto c : coll) {
        c->forceEnable();
    }
    for (auto o : children) {
        o->forceEnable();
    }
}

void Object::onDisable() {
    auto coll = components.getCollection();
    for (auto c : coll) {
        c->forceDisable();
    }
    for (auto o : children) {
        o->forceDisable();
    }
}

void Object::onPreUpdate() {
    auto coll = components.getCollection();
    for (auto c : coll) {
        c->preUpdate();
    }
    for (auto o : children) {
        o->preUpdate();
    }
}

// update all components and child objects
void Object::onUpdate() {
    auto coll = components.getCollection();
    for (auto c : coll) {
        c->update();
    }
    for (auto o : children) {
        o->update();
    }
}

ptr<Object> Object::createEmpty(String name) {
    ptr<Object> obj{new Object(name)};
    getScene()->Register(obj);
    obj->init(shared_from_this());
    children.push_back(obj);
    return obj;
}

ptr<Object> Object::createEnabled(String name) {
    auto o = createEmpty(name);
    o->enable();
    return o;
}

ptr<Object> Object::getParent() {
    auto parent_lock = parent.lock();
    if (!parent_lock)
        THROW_NULL_PTR_EXCEPTION(parent_lock.get());
    return parent_lock;
}

ptr<SceneObjectBase> Object::getBaseParent() {
    return std::static_pointer_cast<SceneObjectBase>(parent.lock());
}

void Object::addComponent(ptr<Components::PackableComponent> component) {
    components.registerPtr(component);
    getScene()->Register(component);
    component->init(shared_from_this());
    component->enable();
}

ObjectData Object::pack() {
    auto parent_lock = parent.lock();

    ObjectData data;
    data.enabled = isEnabled();
    data.selfUUID = getUUID();
    if (parent_lock)
        data.parentUUID = parent_lock->getUUID();
    else
        data.parentUUID = 0;

    uint n = children.size();
    data.childrenUUID.resize(n);
    for (uint i = 0; i < n; i++) {
        data.childrenUUID[i] = children[i]->getUUID();
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
ObjectData::unpack(ptr<SceneBase> scene,
                   std::function<ObjectData(UUID)> getRelativesData) {
    auto parent =
        std::dynamic_pointer_cast<Object>(scene->getRegistered(parentUUID));

    if (!parent) {
        parent = getRelativesData(parentUUID).unpack(scene, getRelativesData);
    }

    auto self_obj = unpack(parent, getRelativesData);

    return self_obj;
}

ptr<Object>
ObjectData::unpack(ptr<Object> parent,
                   std::function<ObjectData(UUID)> getRelativesData) {
    ptr<Object> inst{new Object(name)};
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