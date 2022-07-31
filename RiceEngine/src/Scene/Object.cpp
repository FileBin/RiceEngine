#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/PackableComponent.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/defines.h"

NSP_ENGINE_BEGIN

Object::Object(ptr<Scene> scene) : scene(scene) {}

ptr<Object> Object::create(ptr<Scene> scene) {
    return ptr<Object>(new Object(scene));
}

ptr<Object> Object::createEmpty() {
    ptr<Object> obj{new Object(getScene())};
    obj->parent = shared_from_this();
    children.push_back(obj);
    return obj;
}

ptr<Scene> Object::getScene() {
    auto scene_lock = scene.lock();
    if (!scene_lock)
        THROW_NULL_PTR_EXCEPTION(scene_lock.get());
    return scene_lock;
}

void Object::addComponent(ptr<Components::PackableComponent> component) {
    components.registerPtr(component);
}

ObjectData Object::pack() {
    auto parent_lock = parent.lock();

    ObjectData data;
    data.active = active;
    data.enabled = enabled;
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
    ptr<Object> inst{new Object(parent->getScene())};
    inst->active = active;
    inst->enabled = enabled;
    inst->name = name;
    inst->parent = parent;
    inst->selfUUID = selfUUID;

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