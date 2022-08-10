#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN
class SceneBase;
class SceneRender;
class Object;
class EngineBase;
class ClientEngine;
class ClientScene;
NSP_ENGINE_END

#pragma once
NSP_ENGINE_BEGIN

class SceneObjectBase {
  private:
    friend class SceneBase;
    wptr<SceneBase> scene;
    UUID uuid;

  protected:
    typedef uint flags_t;
    struct Flags {
        constexpr static flags_t NONE = 0x0;
        constexpr static flags_t NEED_ENABLE = 0x1;
        constexpr static flags_t NEED_DISABLE = 0x2;
        constexpr static flags_t NEED_DESTROY = 0x8000;
        constexpr static flags_t ENABLED = 0x10000;
    };
    flags_t flags = 0;

    SceneObjectBase();
    virtual ~SceneObjectBase();

    ptr<EngineBase> getEngine();
    ptr<ClientEngine> getClientEngine();
    ptr<SceneRender> getSceneRender();

    void preUpdate();
    void update();

    virtual void onEnable() = 0;
    virtual void onDisable() = 0;
    virtual void onUpdate() = 0;
    virtual void onPreUpdate() = 0;

    bool canEnable();
    virtual ptr<SceneObjectBase> getBaseParent() = 0;

  public:
    bool isEnabled();

    void forceEnable();
    void forceDisable();

    void enable();
    void disable();

    ptr<SceneBase> getScene();
    ptr<ClientScene> getClientScene();
    UUID getUUID();
};

NSP_ENGINE_END