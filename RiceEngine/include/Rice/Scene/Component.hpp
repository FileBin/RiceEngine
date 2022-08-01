#include "../stdafx.hpp"
#include "Rice/Util/Event.hpp"

NSP_COMPONENTS_BEGIN

class Component;

NSP_COMPONENTS_END

#pragma once

NSP_ENGINE_BEGIN
class Scene;
class SceneRender;
class Object;
class Engine;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Component : public enable_ptr<Component>, public IPackable<data_t> {
    friend class Rice::Object;

  private:
    typedef uint flags_t;
    wptr<Object> object;
    EventRegistration stateChangedRegistration;
    EventRegistration updateRegistration;
    EventRegistration enableRegistration;
    EventRegistration disableRegistration;

    void init(ptr<Object> object);

    void forceEnable();
    void forceDisable();

  protected:
    bool isSceneLoaded();

    ptr<Engine> getEngine();
    ptr<Scene> getScene();
    ptr<SceneRender> getSceneRender();

    virtual void onEnable();
    virtual void onDisable();
    virtual void onUpdate();

  protected:
    Component() = default;

  public:
    ptr<Object> getObject();

    void enable();
    void disable();

    bool isEnabled();

    virtual ~Component();
};

NSP_COMPONENTS_END