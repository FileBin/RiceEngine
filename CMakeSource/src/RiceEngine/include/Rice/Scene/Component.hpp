#pragma once

NSP_SCENING_BEGIN

class SceneObject;
typedef RefPtr<SceneObject> pSceneObject;

class Component {
public:
    void preInit(pSceneObject parent);

    virtual ~Component() = 0;

    virtual void onInit() {};

    void enable() { onEnable(); enabled = true; };
    void disable() { onDisable(); enabled = false; };
    virtual void onEnable() {};
    virtual void start() {};
    virtual void preUpdate() {};
    virtual void update() {};
    virtual void onDisable() {};

    bool isEnabled() { return enabled; }
    pSceneObject getSceneObject();
protected:
    bool enabled = false;
private:
    pSceneObject obj = nullptr;
};

NSP_SCENING_END

#include "SceneObject.hpp"
