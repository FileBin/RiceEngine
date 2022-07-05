#pragma once
#include "../stdafx.hpp"

NSP_SCENING_BEGIN

class Scene;
typedef RefPtr<Scene> pScene;

class SceneObject;
typedef RefPtr<SceneObject> pSceneObject;

class Component;
typedef RefPtr<Component> pComponent;

class SceneObject  {
public:
    enum class Flags {
        NONE = 0x0,
        NEED_ENABLE = 0x1,
        NEED_DISABLE = 0x2,
        NEED_DESTROY = 0x40000000,
    };

    SceneObject (pScene scene);
    ~SceneObject();

    bool isEnabled() { return is_enabled; }

    vec<pSceneObject> getChildren();

    template<typename T>
    vec<RefPtr<T>> getComponents() {
        vec<RefPtr<T>> vec = {};
        for (auto& c : components) {
            auto o = dynamic_cast<T*>(c.cget());
            if (o != nullptr) {
                vec.push_back(o);
            }
        }
        return vec;
    }

    pSceneObject getObjectByName(String name);
    pSceneObject getParent() { return parent; }
    bool tryGetObjectByName(String name, pSceneObject& out_object);

    void addComponent(pComponent component);
    void removeComponent(pComponent component);

    pSceneObject instantiate(pSceneObject orig);
    pSceneObject instantiate();
    void removeChild(pSceneObject whatObj);

    void destroy();

    void setName(String name) { this->name = name; }
    pScene getScene();

    void setActive(bool active) {
        is_active = active;
        if (active) {
            enable();
        } else {
            disable();
        }
    }

    void setActiveImmediate(bool active) {
        is_active = active;
        if (active) {
            forceEnable();
        } else {
            forceDisable();
        }
    }
    bool isActive() { return is_active; }

private:

    friend class Scene;

    void enable();
    void forceEnable();
    void preUpdate();
    void update();
    void disable();
    void forceDisable();

    uint flags = 0;

    bool is_active = true;
    bool is_enabled = false;
    pScene scene = nullptr;
    pSceneObject parent = nullptr;
    String name = "";
    vec<pComponent> components{};
    vec<pSceneObject> children{};
};

NSP_SCENING_END

#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Component.hpp"
