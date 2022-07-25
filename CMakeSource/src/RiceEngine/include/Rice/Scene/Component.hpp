#include "../stdafx.hpp"
#include "BetterCpp/Objects/BasePtr.hpp"

NSP_COMPONENTS_BEGIN

class Component;
typedef RefPtr<Component> pComponent;

struct ComponentData;
typedef RefPtr<ComponentData> pComponentData;

NSP_COMPONENTS_END

#pragma once

NSP_ENGINE_BEGIN
class PTR_PROTO(Object);
class PTR_PROTO(Engine);
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Component : public EnableThisRefPtr<Component> ,public IPackable<pComponentData> {
    friend class ComponentData;

  protected:
    pEngine getEngine();

    pObject object;
    virtual void onInit();
    virtual void onEnable();
    virtual void onDisable();
    virtual void start();
    virtual void preUpdate();
    virtual void update();

  public:
    pObject getObject();

    void enable();
    void disable();

    bool isEnabled();

    virtual ~Component() {}
};

struct ComponentData : public IPackable<data_t> {
    UUID objectUUID;

    ComponentData(pComponent component);

    virtual pComponent createComponent() = 0;
    virtual ~ComponentData() {}
};

NSP_COMPONENTS_END