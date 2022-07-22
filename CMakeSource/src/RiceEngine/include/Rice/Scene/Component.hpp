#include "../stdafx.hpp"

NSP_COMPONENTS_BEGIN

class Component;
typedef RefPtr<Component> pComponent;

struct ComponentData;
typedef RefPtr<ComponentData> pComponentData;


NSP_COMPONENTS_END

#pragma once

#include "Object.hpp"

NSP_COMPONENTS_BEGIN

class Component : public IPackable<pComponentData> {
protected:
  pObject object;

  virtual void onEnable() = 0;
  virtual void start(){};
  virtual void preUpdate(){};
  virtual void update(){};

public:
  pObject getObject();

  void enable();
  void disable();

  bool isEnabled();
};

struct ComponentData : public IPackable<data_t> {
    UUID objectUUID;

    virtual pComponent createComponent();
};

NSP_COMPONENTS_END