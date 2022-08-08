#include "../stdafx.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/Util/Event.hpp"

NSP_COMPONENTS_BEGIN

class Component;

NSP_COMPONENTS_END

#pragma once

NSP_ENGINE_BEGIN
class Object;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Component : public SceneObjectBase, public enable_ptr<Component>, public IPackable<data_t> {
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

    ~Component() override;
};

NSP_COMPONENTS_END