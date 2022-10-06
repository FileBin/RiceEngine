#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN

class Object;

struct ObjectData;

NSP_ENGINE_END

#pragma once

#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include <Rice/Engine/Log.hpp>

#include "SceneBase.hpp"

NSP_ENGINE_BEGIN

class Object : public SceneObjectBase, public enable_ptr<Object> {
    friend class SceneBase;
    friend class Components::Component;

  private:
    std::shared_mutex mutex;
    wptr<Object> parent;
    vec<ptr<Object>> children;
    String name;
    vec<ptr<Components::Component>> components;

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

    void addComponent(ptr<Components::Component> component);

  public:
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

NSP_ENGINE_END