#include "../stdafx.hpp"
#include "Rice/defines.h"

NSP_ENGINE_BEGIN

class Object;
typedef RefPtr<Object> pObject;

struct ObjectData;

NSP_ENGINE_END

#pragma once

#include "Component.hpp"
#include "Scene.hpp"

NSP_ENGINE_BEGIN

class Object : IPackable<ObjectData> {
  private:
    UUID selfUUID;

    pObject parent;
    vec<pObject> children;

    bool active;
    bool enabled;
    pScene scene;
    String name;
    vec<Components::pComponent> components;

    friend struct ObjectData;

    static pObject createEmpty();

  public:
    ObjectData pack() override;

    template <typename T> vec<RefPtr<T>> getComponents() {
        vec<RefPtr<T>> vec = {};
        for (auto c : components) {
            auto o = dynamic_cast<T *>(c.get());
            if (o != nullptr) {
                vec.push_back(c);
            }
        }
        return vec;
    }

    template <typename T> RefPtr<T> getComponent() {
        for (auto c : components) {
            auto o = dynamic_cast<T *>(c.get());
            if (o != nullptr) {
                return c;
            }
        }
        return nullptr;
    }
};

struct ObjectData : public IPackable<data_t> {
    bool active;
    bool enabled;
    String name;
    UUID parentUUID, selfUUID;
    vec<UUID> childrenUUID;

    vec<Components::pComponentData> componentsData;

    data_t pack() override;

    static ObjectData unpack(data_t);

    pObject unpack(pScene scene,
                   std::function<ObjectData(UUID)> getRelativesData);

  private:
    pObject unpack(pObject parent,
                   std::function<ObjectData(UUID)> getRelativesData);
};

NSP_ENGINE_END