#include "../stdafx.hpp"
#include "Rice/Util.hpp"

NSP_ENGINE_BEGIN

class PTR_PROTO(Object);

struct ObjectData;

NSP_ENGINE_END

#pragma once

#include "Component.hpp"
#include "Scene.hpp"

NSP_ENGINE_BEGIN

better_class(Object) better_implements(IPackable<ObjectData>) {
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

  public:
    pScene getScene();
    pObject createEmpty();

    UUID getUUID() { return selfUUID; }

    void addComponent(Components::pComponent component);

    Object(pScene scene);

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
            auto o = as<T>(c);
            if (o != nullptr) {
                return o;
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

    data_t pack() override {
        // TODO make packing
        Log::log(Log::Warning, "Packing not implemented");
        return {};
    }

    static ObjectData unpack(data_t);

    pObject unpack(pScene scene,
                   std::function<ObjectData(UUID)> getRelativesData);

  private:
    pObject unpack(pObject parent,
                   std::function<ObjectData(UUID)> getRelativesData);
};

NSP_ENGINE_END