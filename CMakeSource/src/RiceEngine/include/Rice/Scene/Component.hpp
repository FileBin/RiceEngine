#include "../stdafx.hpp"
#include "Rice/namespaces.h"

NSP_COMPONENTS_BEGIN

class Component;
typedef RefPtr<Component> pComponent;

struct ComponentData;
typedef RefPtr<ComponentData> pComponentData;

NSP_COMPONENTS_END

#pragma once

NSP_ENGINE_BEGIN
class PTR_PROTO(Object);
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Component : public IPackable<pComponentData> {
  protected:
    pObject object;
    virtual void onInit() {}
    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void start() {}
    virtual void preUpdate() {}
    virtual void update() {}

  public:
    pObject getObject();

    virtual pComponentData pack() override;

    void enable();
    void disable();

    bool isEnabled();
};

struct ComponentData : public IPackable<data_t> {
    UUID objectUUID;

    virtual pComponent createComponent();
};

NSP_COMPONENTS_END

#include "Object.hpp"