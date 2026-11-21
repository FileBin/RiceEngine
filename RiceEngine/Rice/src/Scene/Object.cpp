#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/Util/Exceptions/ThreadInterruptException.hpp"
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
    std::shared_lock<std::shared_mutex> lock(mutex);

    for (auto &c : components) {
        if (c->flags & Flags::DESTROYED)
            continue;
        c->preUpdate();
    }
    for (auto &o : children) {
        if (o->flags & Flags::DESTROYED)
            continue;
        o->preUpdate();
    }
    // Destroyed components/children are removed afterwards. The previous
    // version stored iterators and erased them one by one, which invalidated
    // every later stored iterator of the same vector; compact instead, keeping
    // the relative order.
    size_t w = 0;
    for (size_t r = 0; r < components.size(); ++r) {
        if (components[r]->flags & Flags::DESTROYED)
            continue;
        if (w != r)
            components[w] = std::move(components[r]);
        ++w;
    }
    components.resize(w);

    w = 0;
    for (size_t r = 0; r < children.size(); ++r) {
        if (children[r]->flags & Flags::DESTROYED)
            continue;
        if (w != r)
            children[w] = std::move(children[r]);
        ++w;
    }
    children.resize(w);
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
    auto scene = getScene();
    if (!scene->isLoaded())
        THROW_THREAD_ITERRUPT_EXCEPTION;

    if (scene->isUpdateRunning()) {
        // Deferred attach: the update pass holds this object's mutex shared and
        // is iterating `children`, so the object is only built and registered
        // here and pushed into the tree at the scene's drain point.
        ptr<Object> obj{new Object(name)};
        scene->Register(obj);
        obj->init(shared_from_this());
        auto self = shared_from_this();
        scene->enqueueDeferred([self, obj]() { self->attachChild(obj); });
        return obj;
    }

    std::unique_lock<std::shared_mutex> lock(mutex);
    ptr<Object> obj{new Object(name)};
    scene->Register(obj);
    obj->init(shared_from_this());
    children.push_back(obj);
    return obj;
}

void Object::attachChild(ptr<Object> child) {
    if (!child || (child->flags & Flags::DESTROYED))
        return;
    std::unique_lock<std::shared_mutex> lock(mutex);
    children.push_back(child);
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
    auto scene = getScene();
    if (scene->isUpdateRunning()) {
        // `components` may be iterated right now (this object's onUpdate, or
        // its parent's preUpdate), so the push_back is deferred to the scene's
        // drain point at the end of the update pass.
        auto self = shared_from_this();
        scene->enqueueDeferred([self, component]() { self->addComponent(component); });
        return;
    }

    components.push_back(component);
    scene->Register(component);
    component->init(shared_from_this());
    component->enable();
}

NSP_ENGINE_END