#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/Util/Exceptions/ThreadInterruptException.hpp"
#include "Rice/Util/String.hpp"
#include <memory>
#include <mutex>

NSP_ENGINE_BEGIN

Object::Object(String name) : name(name) {}

void Object::init(ptr<Object> parent) { this->parent = parent; }

void Object::onEnable() {
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (auto c : components) {
        c->forceEnable();
    }
    for (auto o : children) {
        o->forceEnable();
    }
}

void Object::onDisable() {
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (auto c : components) {
        c->forceDisable();
    }
    for (auto o : children) {
        o->forceDisable();
    }
}

void Object::onPreUpdate() {
    vec<vec<ptr<Components::Component>>::iterator> toRemoveComponents;
    vec<vec<ptr<Object>>::iterator> toRemoveObjects;
    std::shared_lock<std::shared_mutex> lock(mutex);

    for (auto it = components.begin(); it != components.end(); ++it) {
        if ((*it)->flags & Flags::DESTROYED) {
            toRemoveComponents.push_back(it);
            continue;
        }
        (*it)->preUpdate();
    }
    for (auto it = children.begin(); it != children.end(); ++it) {
        if ((*it)->flags & Flags::DESTROYED) {
            toRemoveObjects.push_back(it);
            continue;
        }
        (*it)->preUpdate();
    }
    for (auto it : toRemoveObjects) {
        children.erase(it);
    }
    for (auto it : toRemoveComponents) {
        components.erase(it);
    }
}

// update all components and child objects
void Object::onUpdate() {
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (auto c : components) {
        c->update();
    }
    for (auto o : children) {
        o->update();
    }
}

void Object::onDestroy() {
    forceDisable();
    std::unique_lock<std::shared_mutex> lock(mutex);
    for (auto o : children) {
        o->forceDestroy();
    }
    for (auto c : components) {
        c->forceDestroy();
    }
    getScene()->Unregister(shared_from_this());
}

bool Object::removeChild(UUID uuid) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    for (auto it = children.begin(); it != children.end(); ++it) {
        if ((*it)->getUUID() == uuid) {
            children.erase(it);
            return true;
        }
    }
    return false;
}

bool Object::removeComponent(UUID uuid) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    for (auto it = components.begin(); it != components.end(); ++it) {
        if ((*it)->getUUID() == uuid) {
            components.erase(it);
            return true;
        }
    }
    return false;
}

ptr<Object> Object::createEmpty(String name) {
    if (!getScene()->isLoaded())
        THROW_THREAD_ITERRUPT_EXCEPTION;
    std::unique_lock<std::shared_mutex> lock(mutex);
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

void Object::addComponent(ptr<Components::Component> component) {
    components.push_back(component);
    getScene()->Register(component);
    component->init(shared_from_this());
    component->enable();
}

NSP_ENGINE_END