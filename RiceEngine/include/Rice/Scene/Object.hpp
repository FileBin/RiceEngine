#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/stdafx.hpp"
#include <shared_mutex>

NSP_ENGINE_BEGIN

class Object;

struct ObjectData;

NSP_ENGINE_END

#pragma once

#include <Rice/Engine/Log.hpp>

#include "SceneBase.hpp"
#include <Rice/Scene/PackableComponent.hpp>

NSP_ENGINE_BEGIN

class Object : public SceneObjectBase,
               public enable_ptr<Object>,
               public IPackable<ObjectData> {
    friend class SceneBase;
    friend class Components::Component;

  private:
    std::shared_mutex mutex;
    wptr<Object> parent;
    vec<ptr<Object>> children;
    String name;
    vec<ptr<Components::PackableComponent>> components;

    friend struct ObjectData;

    Object(String name);

    void init(ptr<Object> parent);

    ptr<Object> getParent();

    void onEnable() override;
    void onDisable() override;
    void onUpdate() override;
    void onPreUpdate() override;
    void onDestroy() override;

    ptr<SceneObjectBase> getBaseParent() override;
    bool removeChild(UUID uuid);
    bool removeComponent(UUID uuid);
  public:
    ptr<Object> createEmpty(String name);
    ptr<Object> createEnabled(String name);

    void addComponent(ptr<Components::PackableComponent> component);

  public:
    ObjectData pack() override;

    template <typename T> vec<ptr<T>> getComponents() {
        vec<ptr<T>> vec = {};
        for (auto c : components) {
            auto o = std::dynamic_pointer_cast<T>(c);
            if (o) {
                vec.push_back(o);
            }
        }
        return vec;
    }

    template <typename T> ptr<T> getComponent() {
        for (auto c : components) {
            auto o = std::dynamic_pointer_cast<T>(c);
            if (o) {
                return o;
            }
        }
        return nullptr;
    }
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

    ptr<Object> unpack(ptr<SceneBase> scene,
                       std::function<ObjectData(UUID)> getRelativesData);

  private:
    ptr<Object> unpack(ptr<Object> parent,
                       std::function<ObjectData(UUID)> getRelativesData);
};

NSP_ENGINE_END