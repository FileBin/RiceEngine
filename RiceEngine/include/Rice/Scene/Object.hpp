#include "../stdafx.hpp"
#include "Rice/Util/Event.hpp"

NSP_ENGINE_BEGIN

class Object;

struct ObjectData;

NSP_ENGINE_END

#pragma once

#include "Component.hpp"
#include "Rice/Scene/PackableComponent.hpp"
#include "Rice/Util/RegisterCollection.hpp"
#include "Scene.hpp"

NSP_ENGINE_BEGIN

class Object : public enable_ptr<Object>, public IPackable<ObjectData> {
    friend class Scene;
    friend class Components::Component;

  private:
    UUID selfUUID;

    wptr<Object> parent;
    vec<ptr<Object>> children;

    wptr<Scene> scene;
    String name;
    RegisterCollection<Components::PackableComponent> components;

    EventRegistration stateChangedRegistration;
    EventRegistration updateRegistration;
    EventRegistration enableRegistration;
    EventRegistration disableRegistration;

    friend struct ObjectData;

    Object(ptr<Scene> scene, String name, UUID uuid);

    void init(ptr<Object> parent);
    void update();

    void onEnable();
    void onDisable();

    bool canUpdate();

    ptr<Object> getParent();

  public:
    struct Events {
        ptr<Event<bool>> stateChanged = Event<bool>::create();
        ptr<Event<>> update = Event<>::create();
    } events;
    ptr<Scene> getScene();
    ptr<Object> createEmpty(String name);
    ptr<Object> createEnabled(String name);

    UUID getUUID() { return selfUUID; }

    void addComponent(ptr<Components::PackableComponent> component);

  public:
    void enable();
    void disable();

    bool isEnabled();

    void forceEnable();
    void forceDisable();
    ObjectData pack() override;

    template <typename T> vec<ptr<T>> getComponents() {
        vec<ptr<T>> vec = {};
        for (auto c : components.getCollection()) {
            auto o = std::dynamic_pointer_cast<T>(c);
            if (o) {
                vec.push_back(o);
            }
        }
        return vec;
    }

    template <typename T> ptr<T> getComponent() {
        auto coll = components.getCollection();
        for (auto c : coll) {
            auto o = std::dynamic_pointer_cast<T>(c);
            if (o) {
                return o;
            }
        }
        return nullptr;
    }

    ptr<Components::Component> getComponent(uint comp_id);
};

struct ObjectData : public IPackable<data_t> {
    bool enabled;
    String name;
    UUID parentUUID, selfUUID;
    vec<UUID> childrenUUID;

    data_t componentsData;

    data_t pack() override {
        // TODO make packing
        Log::log(Log::Warning, "Packing not implemented");
        return {};
    }

    static ObjectData unpack(data_t);

    ptr<Object> unpack(ptr<Scene> scene,
                       std::function<ObjectData(UUID)> getRelativesData);

  private:
    ptr<Object> unpack(ptr<Object> parent,
                       std::function<ObjectData(UUID)> getRelativesData);
};

NSP_ENGINE_END