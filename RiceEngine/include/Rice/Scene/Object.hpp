#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Object;

struct ObjectData;

NSP_ENGINE_END

#pragma once

#include "Component.hpp"
#include "Scene.hpp"
#include "Rice/Scene/PackableComponent.hpp"
#include "Rice/Util/RegisterCollection.hpp"

NSP_ENGINE_BEGIN

class Object : public enable_ptr<Object>, public IPackable<ObjectData> {
  private:
    UUID selfUUID;

    wptr<Object> parent;
    vec<ptr<Object>> children;

    bool active;
    bool enabled;
    wptr<Scene> scene;
    String name;
    RegisterCollection<Components::PackableComponent> components;

    friend struct ObjectData;

  public:
    ptr<Scene> getScene();
    ptr<Object> createEmpty();

    UUID getUUID() { return selfUUID; }

    void addComponent(ptr<Components::PackableComponent> component);

  private:
    Object(ptr<Scene> scene);

  public:
    static ptr<Object> create(ptr<Scene> scene);
    ObjectData pack() override;

    template <typename T> vec<ptr<T>> getComponents() {
        vec<ptr<T>> vec = {};
        for (auto c : components.getCollection()) {
            auto o = std::dynamic_pointer_cast<T>(c);
            if (o != nullptr) {
                vec.push_back(c);
            }
        }
        return vec;
    }

    template <typename T> ptr<T> getComponent() {
        for (auto c : components.getCollection()) {
            auto o = std::dynamic_pointer_cast<T>(c);
            if (o != nullptr) {
                return o;
            }
        }
        return nullptr;
    }

    ptr<Components::Component> getComponent(uint comp_id);
};

struct ObjectData : public IPackable<data_t> {
    bool active;
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